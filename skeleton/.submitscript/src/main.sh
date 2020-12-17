#!/usr/bin/env bash

# This function optionally prepends sudo to commands on systems where it exists.
# On some platforms (i.e. an ubuntu docker container) there is no unprivileged user and thus no sudo.
function prepend_sudo() {
  if [ -x "$(command -v sudo)" ]; then
    echo "sudo $*"
  else
    echo "$*"
  fi
}

function get_package_manager() {
  local
  if [ -x "$(command -v apt)" ]; then
    echo "apt"
  else echo ""; fi
}

function run_command() {
  echo "Running '$*'"
  if [[ $* == *"sudo"* ]]; then echo "You may be asked for your root password."; fi
  eval "$*"
}

PACKAGE_MANAGER=$(get_package_manager)

function try_to_remedy_failing_package_installation() {
  local COMMAND
  case $PACKAGE_MANAGER in
  "apt")
    COMMAND=$(prepend_sudo "apt update")
    ;;
  *)
    echo "I can't help you fix this because I do not recognize your package manager. Please reach out to your teacher to report this issue and to get help." && exit
    ;;
  esac

  echo "Trying to fix installation issues for you."
  if ! run_command "$COMMAND"; then
    echo "Trying to fix installation issues failed again. I'm giving up."
    exit
  fi
}

function install_package() {
  echo "Missing package(s) '$*'."

  # Try to identify the system package manager. Currently only recognizes apt.

  local COMMAND
  case $PACKAGE_MANAGER in
  "apt")
    COMMAND=$(prepend_sudo "apt install -y $*")
    ;;
  *)
    echo "No package manager identified. Please install the missing packages manually." && exit
    ;;
  esac

  echo "Package manager identified, attemping automated installation of the missing packages."
  if ! run_command "$COMMAND"; then
    echo "Automated package installation failed."
    try_to_remedy_failing_package_installation

    if ! run_command "$COMMAND"; then
      echo "Automated package installation failed again. I'm giving up."
    fi
  fi

  echo "Automated package installation successful."
}

function install_pip_package() {
  echo "Missing pip-package(s) '$*'."
  local COMMAND="pip3 install pipenv"
  echo "Atemping automated installation of the missing packages."
  echo "Running '$COMMAND'"
  eval "$COMMAND"
}

function ensure_needed_executables_are_installed() {
  if ! command -v python3 &>/dev/null || ! command -v pip3 &>/dev/null; then
    install_package python3 python3-pip
  fi

  if ! command -v pipenv &>/dev/null; then
    install_pip_package pipenv
  fi
}

ensure_needed_executables_are_installed

SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")

cd "$SCRIPTPATH" || exit

echo "Starting submitscript with a pipenv virtual environment."
echo "This will take a moment."
if ! (pipenv install >/dev/null 2>/dev/null); then
  echo "Installing script dependencies with 'pipenv install' failed."
  exit
fi

pipenv run python3 ./main.py
