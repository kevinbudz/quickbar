# OBS (openSUSE Build Service) packaging

Single OBS package `home:kevinbudz/quickbar` builds both formats
(OBS picks the recipe per repository type):

| Repo type | Recipe | Source files on OBS |
|-----------|--------|---------------------|
| RPM (openSUSE, Fedora) | `quickbar.spec` | `quickbar.spec` + `quickbar-1.0.tar.gz` (via `download_files`) |
| DEB (Debian, Ubuntu) | `quickbar.dsc` | `quickbar.dsc` + `debian.*` + same tarball (via `debtransform`) |

## Canonical sources (edit these, not the OBS copies)

- RPM: [`../rpm/quickbar.spec`](../rpm/quickbar.spec)
- DEB: [`../debian/`](../debian/) (`control`, `rules`, `changelog`, `copyright`)
- DSC template: [`quickbar.dsc`](quickbar.dsc) — keep `Version` and
  `Build-Depends` in sync with `../debian/changelog` and `../debian/control`.
- Service: [`../../_service`](../../_service) (`download_files`).

`.gitattributes` excludes the root `debian` / `quickbar.spec` symlinks from
`git archive` tarballs so the upstream `.orig.tar.gz` contains no `debian`
entry that would collide with the `debian/` dir debtransform assembles.

## Upload / update OBS

```bash
# 1. Bump versions in CMakeLists.txt, packaging/rpm/quickbar.spec,
#    packaging/debian/changelog, packaging/obs/quickbar.dsc (all must match).
# 2. Retag: git tag -f v1.0 && git push -f origin v1.0
#    (GitHub regenerates the release tarball without the symlinks.)

# 3. Checkout OBS package and sync files:
osc checkout home:kevinbudz quickbar
cd home:kevinbudz/quickbar
cp /path/to/quickbar/packaging/rpm/quickbar.spec quickbar.spec
cp /path/to/quickbar/packaging/obs/quickbar.dsc quickbar.dsc
cp /path/to/quickbar/_service _service
for f in changelog control copyright rules; do
  cp /path/to/quickbar/packaging/debian/$f debian.$f
done
# NOTE: no debian.compat (obsolete) and no debian.source-format
# (format comes from quickbar.dsc); ensure debian.rules is executable
# in git (chmod +x packaging/debian/rules) — OBS preserves the bit
# from the uploaded debian.rules.
osc addremove
osc commit -m "Release 1.0 ..."
```

## Repository targets (Plasma >= 6.5 required)

- `openSUSE_Tumbleweed` — rolling, has 6.7+ ✅
- `Fedora_44` — has 6.5+ ✅
- `Debian_Testing` / `Debian_Next` (Forky/Sid, 6.7+) ✅
- Dropped: `Fedora_43` (Plasma 6.4.5), `Debian_13` (6.3.5),
  `xUbuntu_24.10`/`25.10` (≤ 6.4.5) — all too old, fail with a clear
  `libplasma-dev >= 6.5` / CMake message.
