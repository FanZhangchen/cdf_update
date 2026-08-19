#!/usr/bin/env python3
"""
Single-slip GND diagnostic: transported GND vs geometric (curl Fp) GND.

Setup (BLP_L400_single_slip.i) uses two IDENTICAL slip systems, so it is
physically a single slip system with total slip  gamma = gamma1 + gamma2.
For a single slip system the plastic deformation gradient is EXACTLY
        Fp = I + gamma (m (x) n)                     (since (m (x) n)^2 = 0)
so the geometric edge GND (curl Fp projected on the edge line direction
l = m x n = -zhat) is
        rho_G,geom = n_x * d_y(gamma) / b  =  d_y(Fp_yx) / (b * m_y)
(since Fp_yx = gamma m_y n_x).  We compare this against the transported net
density
        rho_G,trans = (rho_pos_1 - rho_neg_1) + (rho_pos_2 - rho_neg_2).

No 2x2 projection, no small-strain approximation, no finite-strain
approximation (the Fp = I + gamma (m (x) n) identity is exact for single slip).

Usage:
    python compare_single_slip_gnd.py
"""

import argparse, glob, os
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

# ── Config ──────────────────────────────────────────────────────────────────
VPP_VARS = {
    "rhoep": "rho_edge_pos_1",
    "rhoen": "rho_edge_neg_1",
    "rhop2": "rho_edge_pos_2",
    "rhon2": "rho_edge_neg_2",
    "fp_yx_line": "fp_yx",
    "fp_xy_line": "fp_xy",
    "fp_xx_line": "fp_xx",
    "fp_yy_line": "fp_yy",
}
SLIP_FILE = "single_slip_sys.txt"
BURGERS = 2.57e-7          # mm, matches `burgers` default in CrystalPlasticityBussoUpdate


def read_slip_systems(path):
    """Read the slip file (normal(3) then direction(3) per row), return
    normalized (m, n, l) triples.  l = m x n is the edge line direction."""
    m = []
    n = []
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            vals = [float(x) for x in line.split()]
            nrm = np.array(vals[0:3], dtype=float)
            drt = np.array(vals[3:6], dtype=float)
            nrm /= np.linalg.norm(nrm)
            drt /= np.linalg.norm(drt)
            n.append(nrm)
            m.append(drt)
    n = np.array(n)
    m = np.array(m)
    l = np.cross(m, n)
    l /= np.linalg.norm(l, axis=1, keepdims=True)
    return m, n, l


def find_vpp_csv(base, vpp, directory):
    pats = [os.path.join(directory, f"{base}_{vpp}*.csv"),
            os.path.join(directory, f"*{vpp}*.csv")]
    hits = []
    for p in pats:
        hits += glob.glob(p)
    hits = sorted(set(hits))
    if not hits:
        raise FileNotFoundError(f"No CSV found for VPP '{vpp}' (base='{base}') in {directory}")
    return hits[-1]


def read_line_csv(path, var_name):
    rows = []
    header = None
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split(",")
            if header is None:
                header = [h.strip() for h in parts]
                continue
            rows.append([float(x) for x in parts])
    rows = np.array(rows)
    header = [h.lower() for h in header]

    def col(name):
        for i, h in enumerate(header):
            if h == name:
                return i
        return None

    y_col = col("y")
    v_col = col(var_name.lower())
    if v_col is None:
        for i, h in enumerate(header):
            if h not in ("x", "y", "z", "id"):
                v_col = i
                break
    if y_col is None or v_col is None:
        raise RuntimeError(f"Cannot parse columns from {path} (header={header})")

    y = rows[:, y_col]
    v = rows[:, v_col]
    order = np.argsort(y)
    return y[order], v[order]


def central_diff(y, f):
    df = np.empty_like(f)
    h = np.diff(y)
    df[1:-1] = (f[2:] - f[:-2]) / (h[1:] + h[:-1])
    df[0] = (f[1] - f[0]) / h[0]
    df[-1] = (f[-1] - f[-2]) / h[-1]
    return df


def rel_l2(a, b):
    return np.linalg.norm(a - b) / max(np.linalg.norm(b), 1e-30)


def main():
    ap = argparse.ArgumentParser(description="single-slip transported vs geometric GND")
    ap.add_argument("--base", default="BLP_L400_single_slip_out")
    ap.add_argument("--dir", default=".")
    ap.add_argument("--burgers", type=float, default=BURGERS)
    ap.add_argument("--slip", default=SLIP_FILE)
    ap.add_argument("--out", default="single_slip_gnd.png")
    args = ap.parse_args()

    m, n, l = read_slip_systems(args.slip)
    nss = len(m)
    print(f"Slip systems: {nss}")
    print(f"  m = {np.array2string(m, precision=4)}")
    print(f"  n = {np.array2string(n, precision=4)}")
    print(f"  l = {np.array2string(l, precision=4)}  (l_z = {l[:, 2]})")

    # sanity: the two systems must be identical for this to be single slip
    if not (np.allclose(m[0], m[1]) and np.allclose(n[0], n[1])):
        raise RuntimeError("The two slip systems are NOT identical; this is not a "
                           "single-slip setup.")

    b = args.burgers
    m_y = m[0, 1]           # y-component of slip direction
    n_x = n[0, 0]           # x-component of plane normal

    # ── Load densities and Fp ────────────────────────────────────────────────
    y_ref = None
    rho = {}
    for sign, vpp in (("pos", "rhoep"), ("neg", "rhoen"),
                      ("pos2", "rhop2"), ("neg2", "rhon2")):
        var = VPP_VARS[vpp]
        path = find_vpp_csv(args.base, vpp, args.dir)
        y, val = read_line_csv(path, var)
        if y_ref is None:
            y_ref = y
        else:
            val = np.interp(y_ref, y, val)
        rho[sign] = val

    fp = {}
    for comp, vpp in (("yx", "fp_yx_line"), ("xy", "fp_xy_line"),
                      ("xx", "fp_xx_line"), ("yy", "fp_yy_line")):
        path = find_vpp_csv(args.base, vpp, args.dir)
        y, val = read_line_csv(path, VPP_VARS[vpp])
        fp[comp] = np.interp(y_ref, y, val)

    # ── Transported vs geometric GND ─────────────────────────────────────────
    rho_G_trans = (rho["pos"] - rho["neg"]) + (rho["pos2"] - rho["neg2"])
    rho_G_geom = central_diff(y_ref, fp["yx"]) / (b * m_y)

    # ── diagnostics ──────────────────────────────────────────────────────────
    print("\n--- diagnostics ---")
    for comp in ("xx", "xy", "yx", "yy"):
        print(f"  fp_{comp}: min={fp[comp].min():.5e} max={fp[comp].max():.5e} "
              f"range={fp[comp].max() - fp[comp].min():.5e}")
    print(f"  rho_G_trans: min={rho_G_trans.min():.4e} max={rho_G_trans.max():.4e} "
          f"std={rho_G_trans.std():.4e}")
    print(f"  rho_G_geom : min={rho_G_geom.min():.4e} max={rho_G_geom.max():.4e} "
          f"std={rho_G_geom.std():.4e}")
    print(f"  m_y={m_y:.4f}  n_x={n_x:.4f}  b={b:.3e}")
    print(f"  corr (no sign flip): {np.corrcoef(rho_G_geom, rho_G_trans)[0,1]:+.4f}")

    # ── global sign of curl Fp is a convention; pick the matching sign ──────
    sign = 1.0 if np.dot(rho_G_geom, rho_G_trans) >= 0 else -1.0
    rho_G_geom *= sign

    corr = np.corrcoef(rho_G_geom, rho_G_trans)[0, 1]
    err = rel_l2(rho_G_geom, rho_G_trans)

    print("\n=== single-slip: transported GND  vs  geometric GND (curl Fp) ===")
    print(f"sign convention applied: {sign}")
    print(f"correlation : {corr:+.4f}")
    print(f"rel L2 err  : {err:.3e}")
    print(f"std ratio geom/trans : {rho_G_geom.std()/rho_G_trans.std():.4f}")

    # ── bulk-vs-boundary sweep (localise any mismatch) ───────────────────────
    n = len(y_ref)
    print("\n=== bulk-vs-boundary sweep (trim t samples from each end) ===")
    print(f"{'trim':>5} {'n_kept':>6} {'corr':>9} {'relL2':>9} {'std(geom/trans)':>16}")
    for t in range(0, n // 4 + 1):
        sl = slice(None) if t == 0 else slice(t, n - t)
        a = rho_G_geom[sl]
        cc = rho_G_trans[sl]
        corr_t = np.corrcoef(a, cc)[0, 1] if (a.std() > 0 and cc.std() > 0) else np.nan
        print(f"{t:5d} {len(a):6d} {corr_t:+9.4f} {rel_l2(a, cc):9.3e} "
              f"{a.std()/cc.std():16.4f}")

    # ── plot ────────────────────────────────────────────────────────────────
    fig, ax = plt.subplots(1, 2, figsize=(10, 4.2))

    ax[0].plot(y_ref, rho_G_geom, "k-", lw=2, label=r"$\rho_G$ geometric (curl Fp)")
    ax[0].plot(y_ref, rho_G_trans, "g--", lw=2, label=r"$\rho_G$ transported")
    ax[0].set_xlabel("y (mm)")
    ax[0].set_ylabel(r"$\rho_G$ (1/mm$^2$)")
    ax[0].set_title(f"Single-slip GND (corr={corr:+.3f})")
    ax[0].legend(fontsize=8)

    ax[1].scatter(rho_G_geom, rho_G_trans, s=8, alpha=0.6)
    lim = max(np.abs(rho_G_geom).max(), np.abs(rho_G_trans).max())
    ax[1].plot([-lim, lim], [-lim, lim], "r--", lw=1, label="y = x")
    ax[1].set_xlabel(r"$\rho_G$ geometric (1/mm$^2$)")
    ax[1].set_ylabel(r"$\rho_G$ transported (1/mm$^2$)")
    ax[1].set_title("parity plot")
    ax[1].legend(fontsize=8)
    ax[1].set_aspect("equal", adjustable="box")

    fig.tight_layout()
    fig.savefig(args.out, dpi=150)
    print(f"\nfigure -> {args.out}")

    out_csv = args.out.replace(".png", ".csv")
    data = np.column_stack([y_ref, rho_G_geom, rho_G_trans])
    np.savetxt(out_csv, data, delimiter=",", header="y,rhoG_geom,rhoG_trans", comments="")
    print(f"table  -> {out_csv}")


if __name__ == "__main__":
    main()
