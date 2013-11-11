project(SOIL C)

include_directories(
    include
)
    
add_library(SOIL STATIC
    src/SOIL.c
    src/image_DXT.c
    src/image_helper.c
    src/stb_image_aug.c
    
    src/stbi_DDS_aug_c.h
    src/stbi_DDS_aug.h
    src/stb_image_aug.h
    
    include/SOIL.h)
