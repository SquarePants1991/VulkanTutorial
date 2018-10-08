1. 下载vulkansdk
2. 拷贝vulkansdk到/usr/local
3. header和lib search path
4. libvulkan.1.1.82.dylib 拷贝到项目，rename为libvulkan.1.dylib
5. 设置copyFile Phase
6. 添加libMoltenVK.dylib和MoltenVK_icd.json，设置copyfile
7. icd的"library_path": "./libMoltenVK.dylib",