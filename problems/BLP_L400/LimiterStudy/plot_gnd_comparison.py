#!/usr/bin/env python3
"""Three-panel transported-vs-geometric GND comparison under mesh refinement.

Each column is one resolution (ny = 100/200/400).  Every panel overlays
        rho_G,trans = rho_pos_1 - rho_neg_1                      (transported)
        rho_G,geom  = d_y(Fp_yx) / (b * m_y)                     (curl Fp)
sign-aligned, against y.

Top row: full domain — the residual mismatch localises to the top/bottom pile-up
boundary layers.  Bottom row: interior zoom (boundary layers excluded), where
the two fields are seen to converge as ny increases.

Reuses the same loaders and the same central-difference geometric reconstruction
as compare_single_slip_gnd.py, so the numbers in the panel titles match that
script and convergence_gnd.py exactly.

Usage (run from the LimiterStudy directory so the import resolves):
    python plot_gnd_comparison.py
"""

import argparse
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

import compare_single_slip_gnd as cg

CASES = [
    (100, "BLP_L400_single_slip_out"),
    (200, "BLP_L400_single_slip_n200_out"),
    (400, "BLP_L400_single_slip_n400_out"),
]
INTERIOR_Y = (0.05, 0.35)   # zoom window excluding the top/bottom boundary layers


def load_case(base, directory, burgers, slip):
    """Return (y, rho_G_trans, rho_G_geom, l1_ratio, corr) for one run."""
    m, n, l = cg.read_slip_systems(slip)
    if len(m) != 1:
        raise RuntimeError("single slip required")
    m_y = m[0, 1]
    b = burgers

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

    path = cg.find_vpp_csv(base, "fp_yx_line", directory)
    y, fp_yx = cg.read_line_csv(path, cg.VPP_VARS["fp_yx_line"])
    fp_yx = np.interp(y_ref, y, fp_yx)

    trans = rho["pos"] - rho["neg"]
    geom = cg.central_diff(y_ref, fp_yx) / (b * m_y)
    # global sign of curl Fp is a convention; align to the transported field
    sign = 1.0 if np.dot(geom, trans) >= 0 else -1.0
    geom *= sign

    _trapz = getattr(np, "trapezoid", getattr(np, "trapz"))
    l1 = _trapz(np.abs(trans), y_ref) / _trapz(np.abs(geom), y_ref)
    corr = np.corrcoef(geom, trans)[0, 1]
    return y_ref, trans, geom, l1, corr


def main():
    ap = argparse.ArgumentParser(description="3-panel transported-vs-geometric GND")
    ap.add_argument("--dir", default=".")
    ap.add_argument("--burgers", type=float, default=cg.BURGERS)
    ap.add_argument("--slip", default=cg.SLIP_FILE)
    ap.add_argument("--out", default="gnd_comparison.png")
    args = ap.parse_args()

    fig, axes = plt.subplots(2, len(CASES), figsize=(4.6 * len(CASES), 8.0))

    for j, (ny, base) in enumerate(CASES):
        try:
            y, trans, geom, l1, corr = load_case(
                base, args.dir, args.burgers, args.slip)
        except FileNotFoundError as e:
            print(f"[skip] ny={ny:4d}  no CSV yet: {e}")
            for row in (0, 1):
                axes[row, j].set_visible(False)
            continue

        # top row: full domain
        ax = axes[0, j]
        ax.plot(y, geom, "k-", lw=2, label="geometric (curl Fp)")
        ax.plot(y, trans, "g--", lw=2, label="transported")
        ax.set_title(f"ny = {ny}\n"
                     r"$r_{L1}$=" + f"{l1:.1f},  corr={corr:+.2f}", fontsize=9)
        ax.set_xlim(0.0, 0.4)
        ax.tick_params(labelbottom=False)
        if j == 0:
            ax.set_ylabel(r"$\rho_G$ (1/mm$^2$)")

        # bottom row: interior zoom
        ax = axes[1, j]
        ax.plot(y, geom, "k-", lw=2)
        ax.plot(y, trans, "g--", lw=2)
        ax.set_xlim(*INTERIOR_Y)
        ax.set_xlabel("y (mm)")
        if j == 0:
            ax.set_ylabel(r"$\rho_G$ (1/mm$^2$)")

    axes[0, 0].legend(fontsize=8, loc="upper right")
    fig.suptitle("transported vs geometric GND under mesh refinement\n"
                 "(bottom row: interior zoom, boundary layers excluded)",
                 fontsize=11)
    fig.tight_layout(rect=[0, 0, 1, 0.94])
    fig.savefig(args.out, dpi=150)
    print(f"figure -> {args.out}")


if __name__ == "__main__":
    main()
