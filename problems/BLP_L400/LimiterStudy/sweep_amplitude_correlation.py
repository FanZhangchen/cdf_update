#!/usr/bin/env python3
"""Physical correlation between curl Fp and transported GND, in two sweeps.

Both modes compare the L1 GND content of the geometric field
        rho_G,geom = d_y(Fp_yx)/(b m_y)   (= curl Fp / b)
against the transported field
        rho_G,trans = rho_pos_1 - rho_neg_1,
over the FULL domain and over an INTERIOR window (boundary pile-up layers
excluded).  The interior content is the clean physical-correlation signal: the
large full-domain ratio R ~ 15-21 is dominated by the under-resolved boundary
layers, whereas the interior R is much closer to 1.

Two modes (--mode):

  amplitude (default)  Sweep the applied shear amplitude (x0.50 ... x1.50) on a
                       fixed mesh.  Shows G_geom and G_trans co-vary on a line
                       through the origin -> same physical field; the residual
                       full-domain ratio drifts with amplitude -> the discretisation
                       error also scales with loading.
                       2x2 figure: full-domain & interior, each (correlation, R vs
                       amplitude).

  mesh                 Sweep the mesh resolution (ny = 100/200/400) at the fixed
                       baseline amplitude.  Shows the interior ratio R_int -> 1 as
                       h -> 0, directly confirming the interior residual is also a
                       first-order discretisation artefact (not a model error).

Usage (run from the LimiterStudy directory so the imports resolve):
    python sweep_amplitude_correlation.py               # amplitude sweep
    python sweep_amplitude_correlation.py --mode mesh   # mesh sweep
"""

import argparse
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

import compare_single_slip_gnd as cg
from plot_gnd_comparison import load_case   # (y, trans, geom, l1, corr) — raw

# (label, base, amplitude_multiplier) — 1.00x reuses the already-run baseline
SWEEP = [
    ("0.50x", "BLP_L400_single_slip_amp50_out",  0.50),
    ("0.75x", "BLP_L400_single_slip_amp75_out",  0.75),
    ("1.00x", "BLP_L400_single_slip_out",        1.00),
    ("1.25x", "BLP_L400_single_slip_amp125_out", 1.25),
    ("1.50x", "BLP_L400_single_slip_amp150_out", 1.50),
]
# (ny, base) — mesh refinement at the fixed baseline amplitude
MESH_CASES = [
    (100, "BLP_L400_single_slip_out"),
    (200, "BLP_L400_single_slip_n200_out"),
    (400, "BLP_L400_single_slip_n400_out"),
]
INTERIOR_Y = (0.05, 0.35)   # excludes the top/bottom pile-up boundary layers
YM = 0.4                    # ymax (mm) from the input file


def _trapz():
    return getattr(np, "trapezoid", getattr(np, "trapz"))


def content(y, field, ylo=None, yhi=None):
    """L1 (absolute) content of `field` over the (optionally masked) y window."""
    m = np.ones_like(y, dtype=bool)
    if ylo is not None:
        m &= y >= ylo
    if yhi is not None:
        m &= y <= yhi
    return _trapz()(np.abs(field[m]), y[m])


def fit_slope(G_trans, G_geom):
    """Least-squares slope of G_geom = s * G_trans through the origin."""
    return float(np.dot(G_trans, G_geom) / np.dot(G_trans, G_trans))


def run_amplitude(args):
    labels, mults = [], []
    T_full, G_full, T_int, G_int = [], [], [], []   # transported / geometric contents
    for label, base, mult in SWEEP:
        try:
            y, trans, geom, l1, corr = load_case(base, args.dir, args.burgers, args.slip)
        except FileNotFoundError as e:
            print(f"[skip] {label:>5}  no CSV yet: {e}")
            continue
        labels.append(label)
        mults.append(mult)
        T_full.append(content(y, trans))
        G_full.append(content(y, geom))
        T_int.append(content(y, trans, *INTERIOR_Y))
        G_int.append(content(y, geom, *INTERIOR_Y))
        print(f"{label:>5}  R_full={T_full[-1]/G_full[-1]:5.2f}  "
              f"R_int={T_int[-1]/G_int[-1]:5.2f}")

    if len(T_full) < 2:
        print("\nneed >= 2 amplitudes to assess correlation.")
        return

    T_full = np.array(T_full); G_full = np.array(G_full)
    T_int = np.array(T_int);   G_int = np.array(G_int)
    mults = np.array(mults)

    s_full = fit_slope(T_full, G_full)
    s_int = fit_slope(T_int, G_int)
    R_full = T_full / G_full
    R_int = T_int / G_int

    def r2(s, Tx, Gx):
        r = Gx - s * Tx
        return 1.0 - float(np.dot(r, r) / np.dot(Gx, Gx))

    print("\n=== correlation (line through origin) ===")
    print(f"  FULL     slope s={s_full:.4f}  R^2={r2(s_full, T_full, G_full):.4f}  "
          f"R={np.array2string(R_full, precision=1)}")
    print(f"  INTERIOR slope s={s_int:.4f}  R^2={r2(s_int, T_int, G_int):.4f}  "
          f"R={np.array2string(R_int, precision=1)}")

    fig, axes = plt.subplots(2, 2, figsize=(11, 8.2))

    def corr_panel(ax, Tx, Gx, s, title):
        ax.plot(Tx, Gx, "ko", ms=8)
        xfit = np.linspace(0, Tx.max() * 1.05, 50)
        ax.plot(xfit, s * xfit, "r-", lw=2, label=f"slope={s:.3f}")
        for lab, gx, gy in zip(labels, Tx, Gx):
            ax.annotate(lab, (gx, gy), textcoords="offset points",
                        xytext=(6, 4), fontsize=8)
        ax.set_xlabel(r"transported GND  $\int|\rho_G|\,dy$  (1/mm)")
        ax.set_ylabel(r"geometric GND  $\int|\mathrm{curl}\,F^p/b|\,dy$  (1/mm)")
        ax.set_title(title)
        ax.legend(fontsize=8)

    def ratio_panel(ax, R, title):
        ax.plot(mults, R, "ks-", ms=6)
        ax.set_xlabel("applied shear amplitude (x baseline)")
        ax.set_ylabel(r"$R = \int|\rho_G|\,/\,\int|\mathrm{curl}\,F^p/b|$")
        ax.set_title(title)
        ax.grid(True, ls="--", alpha=0.4)

    corr_panel(axes[0, 0], T_full, G_full, s_full, "full domain (correlation)")
    ratio_panel(axes[0, 1], R_full, "full domain (R vs amplitude)")
    corr_panel(axes[1, 0], T_int, G_int, s_int,
               f"interior y in {INTERIOR_Y} (correlation)")
    ratio_panel(axes[1, 1], R_int, "interior (R vs amplitude)")

    fig.tight_layout()
    fig.savefig(args.out, dpi=150)
    print(f"\nfigure -> {args.out}")

    out_csv = args.out.replace(".png", ".csv")
    np.savetxt(out_csv, np.column_stack(
                   [mults, T_full, G_full, R_full, T_int, G_int, R_int]),
               delimiter=",",
               header="amplitude,G_trans_full,G_geom_full,R_full,"
                      "G_trans_int,G_geom_int,R_int",
               comments="")
    print(f"table  -> {out_csv}")


def run_mesh(args):
    nys, hs, R_full, R_int = [], [], [], []
    for ny, base in MESH_CASES:
        try:
            y, trans, geom, l1, corr = load_case(base, args.dir, args.burgers, args.slip)
        except FileNotFoundError as e:
            print(f"[skip] ny={ny:4d}  no CSV yet: {e}")
            continue
        T_full = content(y, trans)
        G_full = content(y, geom)
        T_int = content(y, trans, *INTERIOR_Y)
        G_int = content(y, geom, *INTERIOR_Y)
        nys.append(ny)
        hs.append(YM / ny)
        R_full.append(T_full / G_full)
        R_int.append(T_int / G_int)
        m = (y >= INTERIOR_Y[0]) & (y <= INTERIOR_Y[1])
        S_int_trans = _trapz()(trans[m], y[m])   # signed interior content (no abs)
        S_int_geom = _trapz()(geom[m], y[m])
        osc_t = abs(S_int_trans) / T_int         # ~0 => sign-oscillating, ~1 => single-signed
        osc_g = abs(S_int_geom) / G_int
        print(f"ny={ny:4d}  h={hs[-1]:.6f}  R_full={R_full[-1]:5.2f}  "
              f"R_int={R_int[-1]:5.2f}")
        print(f"      interior L1: trans={T_int:.3e}  geom={G_int:.3e}  | "
              f"osc trans={osc_t:.2f}  geom={osc_g:.2f}  "
              f"(signed trans={S_int_trans:+.3e}  geom={S_int_geom:+.3e})")

    if len(nys) < 2:
        print("\nneed >= 2 resolutions to assess interior convergence.")
        return

    nys = np.array(nys, dtype=float)
    hs = np.array(hs)
    R_full = np.array(R_full)
    R_int = np.array(R_int)

    # fit interior excess -> 0:  R_int - 1 = C_int * h^p  (log-log, cf. convergence_gnd)
    exc_int = R_int - 1.0
    if np.all(exc_int > 0):
        A = np.vstack([np.log(hs), np.ones_like(hs)]).T
        (p, logC), *_ = np.linalg.lstsq(A, np.log(exc_int), rcond=None)
        C = np.exp(logC)
        print("\n=== interior convergence fit  R_int - 1 = C h^p ===")
        print(f"  p = {p:+.3f}   (1 = first-order, CONSTANT-MONOMIAL Fp)")
        print(f"  C = {C:.4e}")
        for ny, h, r in zip(nys, hs, R_int):
            print(f"  ny={int(ny):4d}  h={h:.6f}  R_int={r:6.2f}  "
                  f"R_int_fit={1.0 + C * h ** p:6.2f}")
    else:
        p = C = None
        print("\n(interior R_int <= 1 at some resolution; no log-log fit)")

    fig, ax = plt.subplots(figsize=(6.6, 4.6))
    ax.semilogx(nys, R_full, "ko-", ms=7, label=r"$R_{\mathrm{full}}$")
    ax.semilogx(nys, R_int, "bs-", ms=7, label=r"$R_{\mathrm{int}}$ (interior)")
    ax.axhline(1.0, color="r", ls="--", lw=1, label="R = 1 (continuum)")
    for ny, rf, ri in zip(nys, R_full, R_int):
        ax.annotate(f"ny={int(ny)}", (ny, rf), textcoords="offset points",
                    xytext=(6, 4), fontsize=8)
    ax.set_xlabel("ny (elements)")
    ax.set_ylabel(r"$R = \int|\rho_G^{trans}|\,/\,\int|\rho_G^{geom}|$")
    ax.set_title("mesh refinement: full vs interior R (fixed baseline amplitude)")
    ax.grid(True, which="both", ls="--", alpha=0.4)
    ax.legend(fontsize=9)
    fig.tight_layout()
    fig.savefig(args.out, dpi=150)
    print(f"\nfigure -> {args.out}")

    out_csv = args.out.replace(".png", ".csv")
    np.savetxt(out_csv, np.column_stack([nys, hs, R_full, R_int]),
               delimiter=",", header="ny,h,R_full,R_int", comments="")
    print(f"table  -> {out_csv}")


def main():
    ap = argparse.ArgumentParser(description="curl Fp vs transported GND correlation")
    ap.add_argument("--mode", choices=["amplitude", "mesh"], default="amplitude",
                    help="amplitude sweep (fixed mesh) or mesh sweep (fixed amplitude)")
    ap.add_argument("--dir", default=".")
    ap.add_argument("--burgers", type=float, default=cg.BURGERS)
    ap.add_argument("--slip", default=cg.SLIP_FILE)
    ap.add_argument("--out", default=None,
                    help="output figure (default: amplitude_correlation.png / mesh_correlation.png)")
    args = ap.parse_args()

    if args.out is None:
        args.out = ("amplitude_correlation.png" if args.mode == "amplitude"
                    else "mesh_correlation.png")

    if args.mode == "amplitude":
        run_amplitude(args)
    else:
        run_mesh(args)


if __name__ == "__main__":
    main()
