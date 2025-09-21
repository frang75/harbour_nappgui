# Full update of all projects

## Update Harbour

```
# Check remotes

git remote -v
origin  https://github.com/frang75/harbour_nappgui.git (fetch)
origin  https://github.com/frang75/harbour_nappgui.git (push)
upstream        https://github.com/harbour/core.git (fetch)
upstream        https://github.com/harbour/core.git (push)

# Fetch original repo

git fetch upstream

# Check branch in local fork

git branch
  Sprint-90
* Sprint-91
  master
  master-2

# Merge desired commit (from `/harbour/core`) into current branch

git merge c4b2a030c4434381fd3452e4207358282886a191
```

## Update AWS-SDK-CPP

- Go to `/contrib/hbaws/awssdk.bat[.sh]`.
- Change the download branch `git clone --recurse-submodules --depth 1 --branch 1.11.652`.
- Run the script.

## Rebuild all projects

- Go to `/nap-dev`.
- `bash ./build_all.sh -comp gcc -b Release`

