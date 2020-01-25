cd ./Shaders
/Library/vulkansdk/macOS/bin/glslc -c *.vert
/Library/vulkansdk/macOS/bin/glslc -c *.frag

python3 ../SPV.py Default.vert.spv DefaultVertexShader
python3 ../SPV.py Default.frag.spv DefaultFragmentShader
cd ..
