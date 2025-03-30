#@IgnoreInspection BashAddShebang
# needs to be sourced from a bash configuration
# then, when new shell is started and IOTEMPOWER_ROOT set then environment is
# activated
#
# if this is sourced from $HOME/.bashrc, it makes the prompt work in tmux
# and byobu

if [[ "$IOTEMPOWER_ROOT" ]]; then # only load when IOTEMPOWER_ROOT is defined

# handles multile imports on its own (could make sense for defining python or nvm)
source "$IOTEMPOWER_ROOT/lib/shell_starter/config.bash" \
        || echo "trouble initializing, environment might not work." 1>&2

# not necessary with iotknit being installed via pip
# export PYTHONPATH="$PYTHONPATH:$IOTEMPOWER_ROOT/lib" 

## if you type cd you go to iotempower-root
#iot_cd() {
#    IOTEMPOWER_HOMEBACK="$HOME"
#    HOME="$IOTEMPOWER_ROOT"
#    cd "$@"
#    HOME="$IOTEMPOWER_HOMEBACK"
#    unset IOTEMPOWER_HOMEBACK
#}
#
#alias cd=iot_cd
#
#cd

if [[ "$TERM" ]]; then # only mess with the prompt if there is TERM

##### always reset prompt, when IOTEMPOWER_ROOT defined and sourced
PSGREEN="\[$(tput setaf 2 2>/dev/null)\]"
PSLBLUE="\[$(tput setaf 14 2>/dev/null)\]"
PSWHITE="\[$(tput setaf 15 2>/dev/null)\]"
PSBLACK="\[$(tput setaf 16 2>/dev/null)\]"
PSDRED="\[$(tput setaf 88 2>/dev/null)\]"
PSWHITEBG="\[$(tput setab 15 2>/dev/null)\]"
PSBOLD="\[$(tput bold 2>/dev/null)\]"
PSRESET="\[$(tput sgr0 2>/dev/null)\]"
#PSRESET='\[\033[0m\]'


# user@host
_IOT_UH="\u@\h${PSRESET}"
# colored seperator after colored iotempower
#_IOT_SEP="${PSWHITEBG}${PSBOLD}${PSDRED}#${PSRESET}"
_IOT_SEP=" "
# colored iotempower
_IOT_IOT="${PSWHITEBG}${PSBOLD}${PSLBLUE}IoT${PSRESET}"
# colored and bold :
_IOT_CLN="${PSBOLD}${PSDRED}:${PSRESET}"
# path
_IOT_PTH="${PSGREEN}\W${PSRESET}"
# prompt
_IOT_PMT="\$${PSRESET} "

# start is always the same (colored IoT - when IoTempower defined)
PS1="$_IOT_IOT"
PS1='$( [[ "$IOTEMPOWER_ACTIVE" = "yes" ]] && echo "'"$PS1"'")'

## allow recording of pwd in byobu/tmux
## (from https://stackoverflow.com/questions/19200589/auto-update-tmux-status-bar-with-active-pane-pwd#19205087)
PS1=$PS1'$( [[ ! "$TMUX" ]] && echo "'"${_IOT_SEP}${_IOT_UH}${_IOT_CLN}${_IOT_PTH}"'")'
PS1=$PS1'$( [[ "$TMUX" ]] && tmux setenv -g TMUX_PWD_$(tmux display -p "#D" 2>/dev/null| tr -d %) "$PWD")'
PS1="${PS1}${_IOT_PMT}"

# disable prompt command output and save prompt-changes, when in iotempower
# but still execute it
PROMPT_COMMAND='if [[ "$IOTEMPOWER_ACTIVE" ]]; then
_IOT_BKUP="$PS1"
{
'"$PROMPT_COMMAND"'
true
} &>/dev/null;
PS1="$_IOT_BKUP"
unset _IOT_BKUP
else
'"$PROMPT_COMMAND"'
true
fi'

unset _IOT_UH _IOT_SEP _IOT_IOT _IOT_PTH _IOT_PMT
unset PSGREEN PSLBLUE PSWHITE PSBLACK PSDRED PSWHITEBG PSBOLD PSRESET

export PS1

fi # does TERM exist

fi # ending: if [[ "$IOTEMPOWER_ROOT" ]]; then
