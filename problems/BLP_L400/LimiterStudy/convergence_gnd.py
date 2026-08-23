#!/usr/bin/env python3
"""Mesh-convergence analysis for the single-slip GND diagnostic.

Reads the same LineValueSampler CSVs as compare_single_slip_gnd.py at several
mesh resolutions, extracts the L1 (absolute) transported/geometric GND ratio,
fits   r - 1 = C * h^p   (h = element size = 0.4/ny)  on log-log axes, and
reports the convergence order p.

If the model were kinematically inconsistent, the ratio would NOT decrease with
refinement; a fitted p ~ 1 (with r -> 1 as h -> 0) is the signature of the
O(h) discretisation error implied by a piecewise-constant (CONSTANT MONOMIAL) Fp.

Usage (run from the LimiterStudy directory so the import resolves):
    python convergence_gnd.py                    # ny = 100, 200, 400
    python convergence_gnd.py --ny 100 200       # any subset (skips missing CSV)
"""

import argparse, os
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

import compare_single_slip_gnd as cg   # reuse loaders, VPP_VARS, central_diff

YM = 0.4                 # ymax (mm) from the input file
CASES = {
    100: "BLP_L400_single_slip_out",
    200: "BLP_L400_single_slip_n200_out",
    400: "BLP_L400_single_slip_n400_out",
}


def compute_metrics(base, directory, burgers, slip):
    """Load one run and return the transported-vs-geometric GND metrics."""
    m, n, l = cg.read_slip_systems(slip)
    if len(m) != 1:
        raise RuntimeError("convergence analysis requires single slip")
    b = burgers
    m_y = m[0, 1]

    y_ref = None
    rho = {}
    for sign, vpp in (("pos", "rhoep"), ("neg", "rhoen")):
        var = cg.VPP_VARS[vpp]
        path = cg.find_vpp_csv(base, vpp, directory)
        y, val = cg.read_line_csv(path, var)
        if y_ref is None:
            y_ref = y
        else:
            val = np.interp(y_ref, y, val)
        rho[sign] = val

    fp = {}
    for comp, vpp in (("yx", "fp_yx_line"),):
        path = cg.find_vpp_csv(base, vpp, directory)
        y, val = cg.read_line_csv(path, cg.VPP_VARS[vpp])
        fp[comp] = np.interp(y_ref, y, val)

    rho_G_trans = rho["pos"] - rho["neg"]
    rho_G_geom = cg.central_diff(y_ref, fp["yx"]) / (b * m_y)

    # global sign of curl Fp is a convention; align to the transported field
    sign = 1.0 if np.dot(rho_G_geom, rho_G_trans) >= 0 else -1.0
    rho_G_geom *= sign

    _trapz = getattr(np, "trapezoid", getattr(np, "trapz"))
    l1_trans = _trapz(np.abs(rho_G_trans), y_ref)
    l1_geom = _trapz(np.abs(rho_G_geom), y_ref)

    return {
        "l1_ratio": l1_trans / l1_geom,
        "corr": np.corrcoef(rho_G_geom, rho_G_trans)[0, 1],
        "std_ratio": rho_G_geom.std() / rho_G_trans.std(),
        "peak_ratio": np.abs(rho_G_trans).max() / np.abs(rho_G_geom).max(),
        "n_pts": len(y_ref),
    }


def main():
    ap = argparse.ArgumentParser(description="single-slip GND mesh convergence")
    ap.add_argument("--ny", type=int, nargs="+", default=[100, 200, 400])
    ap.add_argument("--dir", default=".")
    ap.add_argument("--burgers", type=float, default=cg.BURGERS)
    ap.add_argument("--slip", default=cg.SLIP_FILE)
    ap.add_argument("--out", default="convergence_gnd.png")
    args = ap.parse_args()

    rows = []          # (ny, h, r)
    for ny in args.ny:
        base = CASES[ny]
        try:
            met = compute_metrics(base, args.dir, args.burgers, args.slip)
        except FileNotFoundError as e:
            print(f"[skip] ny={ny:4d}  no CSV yet: {e}")
            continue
        h = YM / ny
        rows.append((ny, h, met))
        print(f"ny={ny:4d}  h={h:.6f}  L1 ratio={met['l1_ratio']:8.3f}  "
              f"corr={met['corr']:+.3f}  std_ratio={met['std_ratio']:.3f}  "
              f"peak_ratio={met['peak_ratio']:.2f}  n_pts={met['n_pts']}")

    if len(rows) < 2:
        print("\nneed >= 2 resolutions to fit the convergence order.")
        return

    nys = np.array([r[0] for r in rows], dtype=float)
    hs = np.array([r[1] for r in rows])
    rs = np.array([r[2]["l1_ratio"] for r in rows])
    excess = rs - 1.0

    if np.any(excess <= 0):
        print("\nwarning: some r <= 1 (geom >= trans); log(r-1) is undefined. "
              "Check the runs — this usually means the CSV is not from this case.")
        return

    # log-log least-squares:  log(r-1) = p log(h) + log C
    A = np.vstack([np.log(hs), np.ones_like(hs)]).T
    (p, logC), *_ = np.linalg.lstsq(A, np.log(excess), rcond=None)
    C = np.exp(logC)

    print("\n=== convergence fit  r - 1 = C h^p ===")
    print(f"  p = {p:+.3f}   (1 = first-order, i.e. CONSTANT-MONOMIAL Fp)")
    print(f"  C = {C:.4e}")
    for ny, h, r in zip(nys, hs, rs):
        pred = 1.0 + C * h ** p
        print(f"  ny={int(ny):4d}  h={h:.6f}  r={r:8.3f}  r_fit={pred:8.3f}")

    # ── plot ────────────────────────────────────────────────────────────────
    h_fit = np.logspace(np.log10(hs.min()) - 0.1, np.log10(hs.max()) + 0.1, 50)
    r_fit = 1.0 + C * h_fit ** p

    fig, ax = plt.subplots(figsize=(5.6, 4.4))
    ax.loglog(hs, excess, "ko", ms=8, label="data")
    ax.loglog(h_fit, C * h_fit ** p, "r-", lw=2, label=fr"fit $p={p:.2f}$")
    for ny, h, r in zip(nys, hs, rs):
        ax.annotate(f"ny={int(ny)}", (h, r - 1.0),
                    textcoords="offset points", xytext=(6, 4), fontsize=8)

    ax.set_xlabel("element size $h$ (mm)")
    ax.set_ylabel(r"excess L1 ratio  $r-1$")
    ax.set_title(r"single-slip GND: $r = \int|\rho_G^{trans}|/\int|\rho_G^{geom}|$")
    ax.grid(True, which="both", ls="--", alpha=0.4)
    ax.legend(fontsize=9)
    fig.tight_layout()
    fig.savefig(args.out, dpi=150)
    print(f"\nfigure -> {args.out}")

    out_csv = args.out.replace(".png", ".csv")
    np.savetxt(out_csv, np.column_stack([nys, hs, rs, 1.0 + C * hs ** p]),
               delimiter=",", header="ny,h,r,r_fit", comments="")
    print(f"table  -> {out_csv}")


if __name__ == "__main__":
    main()
