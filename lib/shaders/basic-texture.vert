#version 300 es
precision highp float;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;                      
                                                
uniform mat4 u_model;                           
uniform mat4 u_view;                            
uniform mat4 u_projection;                      
                                                
out vec3 v_normal; 
out vec3 frag_world_position;
out vec2 tex_coord;                       
                                                
void main()                                  
{               
    frag_world_position =  vec3(u_model * vec4(a_position, 1.0)); 
    v_normal = mat3(transpose(inverse(u_model))) * a_normal;
    tex_coord = a_texcoord;

    gl_Position = u_projection * u_view * vec4(frag_world_position, 1.0); 
         
}                                             
