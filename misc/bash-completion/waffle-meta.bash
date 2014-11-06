# Copyright 2014 Intel Corporation
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# - Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# - Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

function __waffle_compreply_append {
    local possible_completions="$1"
    local x=
    #local i=${#COMPREPLY[@]}

    for x in $possible_completions; do
        if [[ "$x" = "$cur"* ]]; then
            COMPREPLY+=("$x")
        fi
    done
}

function __waffle_ls_symbolic_refs {
    local git_dir=
    local i=

    git_dir=$(git rev-parse --git-dir)
    if [[ -z "$git_dir" ]]; then
        return
    fi

    for i in HEAD ORIG_HEAD FETCH_HEAD MERGE_HEAD; do
        if [[ -e "$git_dir/$i" ]]; then
            echo "$i"
        fi
    done
}

function __waffle_match_refnames {
    case "$cur" in
        refs|refs/*)
            git for-each-ref --format='%(refname)' "${cur%/*}"
            ;;
        *)
            __waffle_ls_symbolic_refs
            git for-each-ref --format='%(refname:short)' \
                refs/heads refs/tags refs/remotes
            ;;
    esac
}

function __waffle_complete_merge {
    local cur="${COMP_WORDS[COMP_CWORD]}"
    local prev="${COMP_WORDS[COMP_CWORD-1]}"
    local opts="--help"

    COMPREPLY=()

    case "$cur" in
        -*)
            COMPREPLY=($(compgen -W "$opts" -- "$cur"))
            ;;
        *)
            __waffle_compreply_append "$(__waffle_match_refnames)"
            ;;
    esac
}

complete -o nospace -F __waffle_complete_merge waffle-merge
