cd "$(dirname "$0")" 
echo "Compiling shaders"
./compile_shader.sh test
./compile_shader.sh skybox
./compile_shader.sh fullscreen
./compile_shader.sh shadow
./compile_shader.sh directional
./compile_shader.sh bloom
./compile_shader.sh blur
./compile_shader.sh physical