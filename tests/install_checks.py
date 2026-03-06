import os


def get_install_check_commands(
    package_name: str, package_manager: str, module: str, local_dir: str
) -> list[str]:
    if package_manager == "binary":
        return [f"which {package_name}"]
    if package_manager == "npm":
        commands = [f"cd {local_dir}/nodejs && npm list {package_name}"]
        if module == "node_red":
            commands.append(f"cd {os.path.expanduser('~')}/.node-red && npm list {package_name}")
        return commands
    raise NotImplementedError("Only installed binaries and npm packages are supported for now")
