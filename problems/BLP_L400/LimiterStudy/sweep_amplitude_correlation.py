#!/usr/bin/env python3
"""Applied-shear sweep: physical correlation between curl Fp and transported GND.

For a set of single-slip runs at different applied shear amplitudes (same mesh,
same material, same boundaries), extract the L1 GND content of the geometric
field  rho_G,geom = d_y(Fp_yx)/(b m_y)  (i.e. curl Fp / b)  and of the transported
field  rho_G,trans = rho_pos_1 - rho_neg_1, and show they move proportionally.

Both the FULL domain and an INTERIOR window (boundary pile-up layers excluded)
are reported.  The interior content is the clean physical-correlation signal:
the large full-domain ratio R ~ 15-21 is dominated by the under-resolved
boundary layers, whereas the interior R is much closer to 1 and flatter in
amplitude.

Layout (2x2):
  top-left : full-domain    G_geom vs G_trans  (line through origin)
  top-right: full-domain    R vs amplitude
  bot-left : interior       G_geom vs G_trans  (line through origin)
  bot-right: interior       R vs amplitude

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

# (label, base, amplitude_multiplier) — 1.00x reuses the already-run baseline
SWEEP = [
    ("0.50x", "BLP_L400_single_slip_amp50_out",  0.50),
    ("0.75x", "BLP_L400_single_slip_amp75_out",  0.75),
    ("1.00x", "BLP_L400_single_slip_out",        1.00),
    ("1.25x", "BLP_L400_single_slip_amp125_out", 1.25),
    ("1.50x", "BLP_L400_single_slip_amp150_out", 1.50),
]
INTERIOR_Y = (0.05, 0.35)   # excludes the top/bottom pile-up boundary layers


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


def main():
    ap = argparse.ArgumentParser(description="shear-amplitude curl Fp vs GND correlation")
    ap.add_argument("--dir", default=".")
    ap.add_argument("--burgers", type=float, default=cg.BURGERS)
    ap.add_argument("--slip", default=cg.SLIP_FILE)
    ap.add_argument("--out", default="amplitude_correlation.png")
    args = ap.parse_args()

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


if __name__ == "__main__":
    main()
