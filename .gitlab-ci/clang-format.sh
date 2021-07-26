#!/bin/bash
# vim: et sts=4 sw=4

set -euo pipefail

# The following approach was shamelessly copied from ci-fairy
# https://gitlab.freedesktop.org/freedesktop/ci-templates/-/blob/master/tools/ci_fairy.py
if [[ "${CI-}" ]]; then
    upstream="${FDO_UPSTREAM_REPO-}"
    if [[ ! "$upstream" ]]; then
        echo "$FDO_UPSTREAM_REPO not set, using local branches to compare"
        upstream="${CI_PROJECT_PATH}"
    fi
    host="${CI_SERVER_HOST}"
    token="${CI_JOB_TOKEN}"
    url="https://gitlab-ci-token:$token@$host/$upstream"

    remote="clang-fairy"
    if ! git remote | grep -qw "$remote"; then
        git remote add "$remote" "$url"
    fi
    git fetch "$remote"
    remote_prefix="$remote/"
else
    remote_prefix=""
fi

branch="${CI_MERGE_REQUEST_DIFF_BASE_SHA-}"
[[ $branch ]] || branch="master"
# End of the shameless copy

# Older versions of clang-format like 6 and 7 print an annoying message.
# "no modified files to format". Newer ones like 12 (earlier?), do not.
formatting_changes=$(git-clang-format-7 --binary "clang-format-7" --commit "$remote_prefix$branch" -q --diff | grep -v "no modified files to format" || true)
[[ "$formatting_changes" ]] || exit 0

echo "ERROR: Formatting issues detected"
echo
echo "Update the codebase as indicated or disable clang-format locally."
echo
echo "Formatting suggestion:"
echo
echo -e "------------\n$formatting_changes\n------------"
echo

exit 1
