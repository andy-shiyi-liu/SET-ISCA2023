FROM ubuntu:latest

# Install system packages
RUN apt-get update && apt-get install -y \
    htop tmux bash-completion sudo \
    git curl wget unzip make \
    build-essential cmake gcc g++ gdb\
    python-is-python3 python3-pip

# Setup user
ARG USERNAME=ubuntu
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Switch to user's home directory
# WORKDIR /home/$USERNAME

# add user
RUN echo "$USERNAME ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

# Switch User
USER $USERNAME

# Configure git
RUN git config --global pull.rebase false && \
    git config --global user.email "andy.shiyi.liu@gmail.com" && \
    git config --global user.name "Andy Shiyi Liu" && \
    git config --global core.editor "code --wait" && \
    git config --global init.defaultBranch main

# Customize .bashrc for the user
RUN echo "export PS1='\\[\\e[1;32m\\]\\u@\\h:\\[\\e[1;34m\\]\\w\\[\\e[0m\\]\\$ '" >> /home/$USERNAME/.bashrc && \
    echo "alias ll='ls -lah'" >> /home/$USERNAME/.bashrc && \
    echo "source /etc/bash_completion" >> /home/$USERNAME/.bashrc && \
    echo "alias t0='tmux attach-session -t 0'" >> /home/$USERNAME/.bashrc

# change vscode max files to watch
RUN echo "fs.inotify.max_user_watches=524288" | sudo tee -a /etc/sysctl.conf \
    && sudo sysctl -p

# enable mouse scroll in tmux
RUN echo "set -g mouse on" >> /home/$USERNAME/.tmux.conf
