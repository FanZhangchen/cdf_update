#!/usr/bin/env python3
"""
Compare kinematic Nye tensor (curl Fp) with transported GND densities.

Theory (rebuttal to Reviewer on "curl Fp vs scalar GND"):

  * The transported field carries signed edge densities rho_edge_pos/neg per
    slip system. The geometrically-necessary (net) density is
        rho_G^a = rho_pos^a - rho_neg^a               (scalar, 1/mm^2)

  * Microscopic Nye tensor (reconstructed from transported densities):
        alpha_trans = sum_a  b * rho_G^a * (m^a (x) l^a)
    with b = Burgers magnitude, m^a = slip direction, l^a = m^a x n^a = edge
    line direction (here both = -zhat, out-of-plane).

  * Kinematic Nye tensor:
        alpha_kin = curl Fp,  (curl Fp)_ij = eps_{jkl} d_k Fp_{il}
    In this 1-column mesh (nx = 1, uniform in x, periodic)  d_x = 0, so the
    only non-zero components are
        alpha_xz = -d_y Fp_xx ,   alpha_yz = -d_y Fp_yx
    exactly the two components spanned by the two edge systems.

  * Because the two slip systems are NOT orthogonal (m1.m2 = cos(2*theta)
    != 0), we must not use the naive projection  rho_G^a = (1/b) alpha:(m^a
    (x) l^a).  Instead solve the 2x2 system  [B^a:B^b] rho_G^b = alpha:B^a / b.

Inputs: LineValueSampler CSVs written by BLP_L400_dg.i (rhoep, rhoen, rhop2,
rhon2, fp_xx_line, fp_xy_line, fp_yx_line, fp_yy_line).

Usage:
    python compare_curlFp_gnd.py                  # defaults below
    python compare_curlFp_gnd.py --base BLP_L400_dg_out --dir .
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
    "fp_xx_line": "fp_xx",
    "fp_xy_line": "fp_xy",
    "fp_yx_line": "fp_yx",
    "fp_yy_line": "fp_yy",
}
SLIP_FILE = "input_slip_sys_al.txt"
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
    l = np.cross(m, n)     # edge line direction for each slip system
    l /= np.linalg.norm(l, axis=1, keepdims=True)
    return m, n, l


def find_vpp_csv(base, vpp, directory):
    """Locate the LineValueSampler CSV for a named VectorPostprocessor.

    MOOSE names them <file_base>_<vpp>[_<num>].csv; the number varies by
    version, so glob and pick the last one (final step for execute_on=final)."""
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
    """Read a LineValueSampler CSV -> (y, value).  Robust to column order."""
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
        # fall back: first column that is not x/y/z/id
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
    """d f/d y by central differences (one-sided at the ends)."""
    df = np.empty_like(f)
    h = np.diff(y)
    df[1:-1] = (f[2:] - f[:-2]) / (h[1:] + h[:-1])
    df[0] = (f[1] - f[0]) / h[0]
    df[-1] = (f[-1] - f[-2]) / h[-1]
    return df


def main():
    ap = argparse.ArgumentParser(description="curl Fp vs transported GND comparison")
    ap.add_argument("--base", default="BLP_L400_dg_out")
    ap.add_argument("--dir", default=".")
    ap.add_argument("--burgers", type=float, default=BURGERS)
    ap.add_argument("--slip", default=SLIP_FILE)
    ap.add_argument("--out", default="curlFp_vs_gnd.png")
    args = ap.parse_args()

    m, n, l = read_slip_systems(args.slip)
    nss = len(m)
    print(f"Slip systems: {nss}  (line dirs l_z = {l[:, 2]})")

    # ── Load transported densities ──────────────────────────────────────────
    y_ref = None
    rho = {}          # rho[('pos', a)] = array over y
    for a in range(nss):
        for sign, vpp in (("pos", f"rhoep" if a == 0 else f"rhop{a+1}"),
                          ("neg", f"rhoen" if a == 0 else f"rhon{a+1}")):
            var = VPP_VARS[vpp]
            path = find_vpp_csv(args.base, vpp, args.dir)
            y, val = read_line_csv(path, var)
            if y_ref is None:
                y_ref = y
            else:
                val = np.interp(y_ref, y, val)
            rho[(sign, a)] = val
            print(f"  loaded {vpp} ({var})  n={len(val)}")

    # ── Load Fp components (CONSTANT MONOMIAL, one value per element) ──────
    fp = {}
    for comp in ("xx", "xy", "yx", "yy"):
        vpp = f"fp_{comp}_line"
        path = find_vpp_csv(args.base, vpp, args.dir)
        y, val = read_line_csv(path, VPP_VARS[vpp])
        fp[comp] = np.interp(y_ref, y, val)

    b = args.burgers

    # ── Kinematic Nye tensor: alpha_xz = -d_y Fp_xx, alpha_yz = -d_y Fp_yx ──
    # (d_x terms vanish: nx=1, periodic, uniform in x)
    alpha_kin_xz = -central_diff(y_ref, fp["xx"])
    alpha_kin_yz = -central_diff(y_ref, fp["yx"])

    # ── Transported Nye tensor: alpha = sum_a b rho_G^a (m^a (x) l^a) ──────
    alpha_trans_xz = np.zeros_like(y_ref)
    alpha_trans_yz = np.zeros_like(y_ref)
    rho_G = {}
    for a in range(nss):
        rho_G[a] = rho[("pos", a)] - rho[("neg", a)]
        alpha_trans_xz += b * rho_G[a] * m[a, 0] * l[a, 2]
        alpha_trans_yz += b * rho_G[a] * m[a, 1] * l[a, 2]

    # ── Global sign of curl Fp is a convention; pick the sign that best ────
    # ── matches the transported reconstruction, and report it.            ───
    lhs = np.concatenate([alpha_kin_xz, alpha_kin_yz])
    rhs = np.concatenate([alpha_trans_xz, alpha_trans_yz])
    sign = 1.0 if np.dot(lhs, rhs) >= 0 else -1.0
    alpha_kin_xz *= sign
    alpha_kin_yz *= sign

    def rel_l2(a, b):
        return np.linalg.norm(a - b) / max(np.linalg.norm(b), 1e-30)

    err_xz = rel_l2(alpha_kin_xz, alpha_trans_xz)
    err_yz = rel_l2(alpha_kin_yz, alpha_trans_yz)

    # ── Per-slip-system GND via the correct 2x2 solve ───────────────────────
    # B^a = m^a (x) l^a ;  M_ab = B^a:B^b = (m^a.m^b)(l^a.l^b)
    M = np.zeros((nss, nss))
    for a in range(nss):
        for c in range(nss):
            M[a, c] = np.dot(m[a], m[c]) * np.dot(l[a], l[c])
    c = np.zeros((nss, len(y_ref)))          # c_a(y) = alpha_kin : B^a  (only xz,yz nonzero)
    for a in range(nss):
        c[a] = (alpha_kin_xz * m[a, 0] * l[a, 2] +
                alpha_kin_yz * m[a, 1] * l[a, 2])
    rho_G_kin = np.linalg.solve(M, c) / b     # shape (nss, n_y)

    # ── Report ───────────────────────────────────────────────────────────────
    print("\n=== curl Fp  vs  transported GND  (sign convention applied:", sign, ") ===")
    print(f"rel L2 err  alpha_xz : {err_xz:.3e}")
    print(f"rel L2 err  alpha_yz : {err_yz:.3e}")
    for a in range(nss):
        err = rel_l2(rho_G_kin[a], rho_G[a])
        corr = np.corrcoef(rho_G_kin[a], rho_G[a])[0, 1]
        print(f"slip {a+1}:  rho_G,kin vs rho_G,trans  relL2={err:.3e}  corr={corr:.3f}")

    # ── Plots ────────────────────────────────────────────────────────────────
    fig, ax = plt.subplots(1, 3, figsize=(15, 4.2))

    ax[0].plot(y_ref, alpha_kin_xz, "r-", lw=2, label=r"$\alpha_{xz}$ (curl Fp)")
    ax[0].plot(y_ref, alpha_trans_xz, "r--", lw=2, label=r"$\alpha_{xz}$ (transp.)")
    ax[0].plot(y_ref, alpha_kin_yz, "b-", lw=2, label=r"$\alpha_{yz}$ (curl Fp)")
    ax[0].plot(y_ref, alpha_trans_yz, "b--", lw=2, label=r"$\alpha_{yz}$ (transp.)")
    ax[0].set_xlabel("y (mm)")
    ax[0].set_ylabel(r"Nye tensor $\alpha$ (1/mm)")
    ax[0].set_title("Kinematic vs transported Nye tensor")
    ax[0].legend(fontsize=8)

    for a in range(nss):
        ax[1 + a].plot(y_ref, rho_G_kin[a], "k-", lw=2, label="kinematic (curl Fp)")
        ax[1 + a].plot(y_ref, rho_G[a], "g--", lw=2, label="transported")
        ax[1 + a].set_xlabel("y (mm)")
        ax[1 + a].set_ylabel(r"$\rho_G$ (1/mm$^2$)")
        ax[1 + a].set_title(f"Slip system {a+1}: GND")
        ax[1 + a].legend(fontsize=8)

    fig.tight_layout()
    fig.savefig(args.out, dpi=150)
    print(f"\nfigure -> {args.out}")

    # dump comparison table for the paper
    out_csv = args.out.replace(".png", ".csv")
    hdr = "y,rhoG_kin_1,rhoG_trans_1,rhoG_kin_2,rhoG_trans_2,alpha_xz_kin,alpha_xz_trans,alpha_yz_kin,alpha_yz_trans"
    data = np.column_stack([y_ref, rho_G_kin[0], rho_G[0], rho_G_kin[1], rho_G[1],
                            alpha_kin_xz, alpha_trans_xz, alpha_kin_yz, alpha_trans_yz])
    np.savetxt(out_csv, data, delimiter=",", header=hdr, comments="")
    print(f"table  -> {out_csv}")


if __name__ == "__main__":
    main()
