#!/usr/bin/env python3
"""Applied-shear sweep: physical correlation between curl Fp and transported GND.

For a set of single-slip runs at different applied shear amplitudes (same mesh,
same material, same boundaries), extract the L1 GND content of the geometric
field  rho_G,geom = d_y(Fp_yx)/(b m_y)  (i.e. curl Fp / b)  and of the transported
field  rho_G,trans = rho_pos_1 - rho_neg_1, and show they move proportionally.

A straight line through the origin means the two quantities respond to the same
physical input in lockstep — the physical correlation.  The slope is the
(amplitude-independent) discretisation factor 1/R, which tends to 1 under mesh
refinement (see convergence_gnd.py).

Left panel : G_geom vs G_trans, with a least-squares line through the origin.
Right panel: the ratio R = G_trans / G_geom vs amplitude — flat means the
             discretisation factor is amplitude-independent (pure numerics).

Usage (run from the LimiterStudy directory so the imports resolve):
    python sweep_amplitude_correlation.py
"""

import argparse
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

import compare_single_slip_gnd as cg
from plot_gnd_comparison import load_case   # (y, trans, geom, l1, corr) — raw

# (label, base) — 1.00x reuses the already-run baseline
SWEEP = [
    ("0.50x", "BLP_L400_single_slip_amp50_out",  0.50),
    ("0.75x", "BLP_L400_single_slip_amp75_out",  0.75),
    ("1.00x", "BLP_L400_single_slip_out",        1.00),
    ("1.25x", "BLP_L400_single_slip_amp125_out", 1.25),
    ("1.50x", "BLP_L400_single_slip_amp150_out", 1.50),
]


def main():
    ap = argparse.ArgumentParser(description="shear-amplitude curl Fp vs GND correlation")
    ap.add_argument("--dir", default=".")
    ap.add_argument("--burgers", type=float, default=cg.BURGERS)
    ap.add_argument("--slip", default=cg.SLIP_FILE)
    ap.add_argument("--out", default="amplitude_correlation.png")
    args = ap.parse_args()

    _trapz = getattr(np, "trapezoid", getattr(np, "trapz"))

    labels, mults = [], []
    G_geom, G_trans, peak_geom, peak_trans = [], [], [], []
    for label, base, mult in SWEEP:
        try:
            y, trans, geom, l1, corr = load_case(base, args.dir, args.burgers, args.slip)
        except FileNotFoundError as e:
            print(f"[skip] {label:>5}  no CSV yet: {e}")
            continue
        labels.append(label)
        mults.append(mult)
        G_geom.append(_trapz(np.abs(geom), y))
        G_trans.append(_trapz(np.abs(trans), y))
        peak_geom.append(np.abs(geom).max())
        peak_trans.append(np.abs(trans).max())
        print(f"{label:>5}  G_trans={G_trans[-1]:.4e}  G_geom={G_geom[-1]:.4e}  "
              f"R={G_trans[-1]/G_geom[-1]:.2f}  peak_ratio={peak_trans[-1]/peak_geom[-1]:.2f}")

    if len(G_trans) < 2:
        print("\nneed >= 2 amplitudes to assess correlation.")
        return

    G_geom = np.array(G_geom)
    G_trans = np.array(G_trans)
    mults = np.array(mults)

    # least-squares line through the origin: G_geom = slope * G_trans
    slope = float(np.dot(G_trans, G_geom) / np.dot(G_trans, G_trans))
    resid = G_geom - slope * G_trans
    r2 = 1.0 - float(np.dot(resid, resid) / np.dot(G_geom, G_geom))
    ratio = G_trans / G_geom

    print("\n=== correlation  G_geom = s * G_trans  (through origin) ===")
    print(f"  slope s = {slope:.4f}   (expected ~1/R ~ 1/{G_trans[0]/G_geom[0]:.1f})")
    print(f"  R^2     = {r2:.4f}   (1 = perfect proportionality)")
    print(f"  per-amplitude R = {np.array2string(ratio, precision=2)}")

    fig, axes = plt.subplots(1, 2, figsize=(11, 4.4))

    ax = axes[0]
    ax.plot(G_trans, G_geom, "ko", ms=8)
    xfit = np.linspace(0, G_trans.max() * 1.05, 50)
    ax.plot(xfit, slope * xfit, "r-", lw=2, label=f"slope={slope:.3f}")
    for lab, gx, gy in zip(labels, G_trans, G_geom):
        ax.annotate(lab, (gx, gy), textcoords="offset points", xytext=(6, 4), fontsize=8)
    ax.set_xlabel(r"transported GND content  $\int|\rho_G|\,dy$  (1/mm)")
    ax.set_ylabel(r"geometric GND content  $\int|\mathrm{curl}\,F^p/b|\,dy$  (1/mm)")
    ax.set_title(rf"physical correlation ($R^2={r2:.4f}$)")
    ax.legend(fontsize=9)

    ax = axes[1]
    ax.plot(mults, ratio, "ks-", ms=6)
    ax.set_xlabel("applied shear amplitude (x baseline)")
    ax.set_ylabel(r"ratio  $R = \int|\rho_G|\,/\,\int|\mathrm{curl}\,F^p/b|$")
    ax.set_title("discretisation factor vs amplitude\n(flat = pure numerics, "
                 "tends to 1 under refinement)")
    ax.grid(True, ls="--", alpha=0.4)

    fig.tight_layout()
    fig.savefig(args.out, dpi=150)
    print(f"\nfigure -> {args.out}")

    out_csv = args.out.replace(".png", ".csv")
    np.savetxt(out_csv, np.column_stack([mults, G_trans, G_geom, ratio, peak_trans, peak_geom]),
               delimiter=",", header="amplitude,G_trans,G_geom,R,peak_trans,peak_geom", comments="")
    print(f"table  -> {out_csv}")


if __name__ == "__main__":
    main()
