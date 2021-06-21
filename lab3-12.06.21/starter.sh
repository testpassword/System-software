case ${1} in
  "--dep")
    echo toor | sudo -S apt-get install libncurses5-dev
    ;;
  "--build")
    cmake CMakeLists.txt
    cmake --build .
    chmod +x mortem
    ;;
  "--client")
  ./mortem --client 127.0.0.1 5555
    ;;
  "--serve")
    ./mortem --serve 5555
    ;;
  *)
    echo "unknown mode"
esac