#@IgnoreInspection BashAddShebang
# needs to be sourced from a bash configuration
# then, when new shell is started and ULNOIOT_ROOT set then environment is
# activated
#
# if this is sourced from $HOME/.bashrc, it makes the prompt work in tmux
# and byobu

if [[ "$ULNOIOT_ROOT" ]]; then # only load when ULNOIOT_ROOT is defined
if [[ ! "$ULNOIOT_ACTIVE" = "yes" ]]; then # only import once

source "$ULNOIOT_ROOT/lib/shell_starter/config.bash" \
        || echo "trouble initializing, environment might not work." 1>&2

export PYTHONPATH="$PYTHONPATH:$ULNOIOT_ROOT/lib"

## if you type cd you go to ulnoiot-root
#ulnoiot_cd() {
#    ULNOIOT_HOMEBACK="$HOME"
#    HOME="$ULNOIOT_ROOT"
#    cd "$@"
#    HOME="$ULNOIOT_HOMEBACK"
#    unset ULNOIOT_HOMEBACK
#}
#
#alias cd=ulnoiot_cd
#
#cd

fi # ending: if [[ ! "$ULNOIOT_ACTIVE" = "yes"]]; then

if [[ "$TERM" ]]; then # only mess with the prompt if there is TERM

##### always reset prompt, when ULNOIOT_ROOT defined and sourced
PSGREEN="\[$(tput setaf 2)\]"
PSLBLUE="\[$(tput setaf 14)\]"
PSWHITE="\[$(tput setaf 15)\]"
PSBLACK="\[$(tput setaf 16)\]"
PSDRED="\[$(tput setaf 88)\]"
PSWHITEBG="\[$(tput setab 15)\]"
PSBOLD="\[$(tput bold)\]"
PSRESET="\[$(tput sgr0)\]"
#PSRESET='\[\033[0m\]'


# user@host
_UIOT_UH="\u@\h${PSRESET}"
# colored seperator after colored ulnoiot
#_UIOT_SEP="${PSWHITEBG}${PSBOLD}${PSDRED}#${PSRESET}"
_UIOT_SEP=" "
# colored ulnoiot
_UIOT_UIOT="${PSWHITEBG}${PSBOLD}${PSBLACK}ul${PSDRED}no${PSBLACK}iot${PSRESET}"
# colored and bold :
_UIOT_CLN="${PSBOLD}${PSDRED}:${PSRESET}"
# path
_UIOT_PTH="${PSGREEN}\W${PSRESET}"
# prompt
_UIOT_PMT="\$${PSRESET} "

# start is always the same (colored ulnoiot - when ulnoiot defined)
PS1="$_UIOT_UIOT"
PS1='$( [[ "$ULNOIOT_ACTIVE" = "yes" ]] && echo "'"$PS1"'")'

## allow recording of pwd in byobu/tmux
## (from https://stackoverflow.com/questions/19200589/auto-update-tmux-status-bar-with-active-pane-pwd#19205087)
PS1=$PS1'$( [[ ! "$TMUX" ]] && echo "'"${_UIOT_SEP}${_UIOT_UH}${_UIOT_CLN}${_UIOT_PTH}"'")'
PS1=$PS1'$( [[ "$TMUX" ]] && tmux setenv -g TMUX_PWD_$(tmux display -p "#D" 2>/dev/null| tr -d %) "$PWD")'
PS1="${PS1}${_UIOT_PMT}"

# disable prompt command output and save prompt-changes, when in ulnoiot
# but still execute it
PROMPT_COMMAND='if [[ "$ULNOIOT_ACTIVE" ]]; then
_UIOT_BKUP="$PS1"
{
'"$PROMPT_COMMAND"'
true
} &>/dev/null;
PS1="$_UIOT_BKUP"
unset _UIOT_BKUP
else
'"$PROMPT_COMMAND"'
true
fi'

unset _UIOT_UH _UIOT_SEP _UIOT_UIOT _UIOT_PTH _UIOT_PMT
unset PSGREEN PSLBLUE PSWHITE PSBLACK PSDRED PSWHITEBG PSBOLD PSRESET

export PS1

fi # does TERM exist

fi # ending: if [[ "$ULNOIOT_ROOT" ]]; then
