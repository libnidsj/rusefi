#!/bin/bash
# rusefi build environment setup

echo "Configurando ambiente de build..."

# Configurar PATH se necessario
if ! command -v arm-none-eabi-gcc >/dev/null 2>&1; then
    if [ -x /opt/gcc-arm-none-eabi/bin/arm-none-eabi-gcc ]; then
        export PATH="/opt/gcc-arm-none-eabi/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/usr/lib/wsl/lib:/mnt/c/Program Files/Eclipse Adoptium/jre-17.0.11.9-hotspot/bin:/mnt/c/WINDOWS/system32:/mnt/c/WINDOWS:/mnt/c/WINDOWS/System32/Wbem:/mnt/c/WINDOWS/System32/WindowsPowerShell/v1.0/:/mnt/c/WINDOWS/System32/OpenSSH/:/mnt/c/Program Files/dotnet/:/mnt/c/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.8:/mnt/c/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.8/bin:/mnt/c/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.8/include:/mnt/c/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.8/lib/x64:/mnt/c/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.8/libnvvp:/mnt/c/Program Files (x86)/NVIDIA Corporation/PhysX/Common:/mnt/c/Program Files/Graphviz/bin:/mnt/c/Program Files/Git/cmd:/mnt/c/Program Files/Simple Device Model:/mnt/c/Program Files/NVIDIA Corporation/NVIDIA app/NvDLISR:/mnt/c/Program Files/Go/bin:/mnt/c/platform-tools:/mnt/c/Android:/mnt/c/Windows/System32:/mnt/c/Users/libni/AppData/Local/Programs/Python/Python310/Scripts/:/mnt/c/Users/libni/AppData/Local/Programs/Python/Python310/:/mnt/c/Users/libni/AppData/Local/Microsoft/WindowsApps:/mnt/c/Users/libni/AppData/Local/Programs/Microsoft VS Code/bin:/mnt/c/Users/libni/AppData/Local/GitHubDesktop/bin:/mnt/c/Users/libni/go/bin:/mnt/c/Users/libni/AppData/Local/Programs/cursor/resources/app/bin"
        echo "ARM GCC Toolchain adicionado ao PATH"
    fi
fi

# Configurar variaveis de build
export MAKEFLAGS="-j12"
export NUMJOBS=" -j12 "

# Funcao para build rapido
function build_rusefi() {
    echo "Iniciando build do rusefi..."
    make clean
    ./bin/compile.sh
}

# Funcao para listar boards
function list_boards() {
    echo "Boards disponiveis:"
    find config/boards -name "meta-info*.env" | sort | head -20
}

# Funcao para build especifico
function build_board() {
    if [ -z "" ]; then
        echo "Uso: build_board config/boards/f407-discovery/meta-info.env"
        return 1
    fi
    echo "Building board: "
    ./bin/compile.sh ""
}

echo "Ambiente configurado!"
echo "Comandos disponiveis:"
echo "  list_boards      - Listar boards"
echo "  build_rusefi     - Build interativo"
echo "  build_board PATH - Build especifico"
echo ""
