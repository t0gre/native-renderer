    #version 300 es 
    precision highp float;  

    uniform vec3 u_view_position; 

    struct BasicTextureMaterial {
        float shininess;
    };

    uniform BasicTextureMaterial u_material;

    struct AmbientLight {
      vec3 color;
    };

    struct DirectionalLight {
      vec3 color;
      vec3 direction;
    };

    struct PointLight {
      vec3 color;
      vec3 position;
      float constant;
      float linear;
      float quadratic;
    };

    struct LightColorComponents {
      vec3 diffuse;
      vec3 specular;
    };

    uniform AmbientLight u_ambient_light;
    uniform DirectionalLight u_directional_light; 
    uniform PointLight u_point_light; 
    uniform sampler2D mesh_texture;                   
                                                  
    in vec3 v_normal;     
    in vec3 frag_world_position;  
    in vec2 tex_coord;


    out vec4 outColor;                        

    LightColorComponents calculateLightComponents(
      vec3 light_color, 
      vec3 light_direction,
      vec3 view_dir,
      vec3 normal,
      vec3 base_color) {    

        // diffuse
        float light_diff = max(dot(light_direction, normal), 0.0);   
        vec3 diffuse_color_component = light_color * light_diff * base_color;

        // specular
        vec3 reflect_dir = reflect(-light_direction, normal);  
        float spec = pow(max(dot(view_dir, reflect_dir), 0.0), u_material.shininess);
        vec3 specular_color_component = light_color * spec;

        return LightColorComponents(diffuse_color_component, specular_color_component);

    }

    uniform sampler2D u_shadowMap;
    uniform mat4 u_lightViewProj;

    float getShadow(vec3 worldPos) {


        vec4 lightSpacePos = u_lightViewProj * vec4(worldPos, 1.0);
        vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
        projCoords = projCoords * 0.5 + 0.5; // to [0,1]

        if(projCoords.x < 0.0 || projCoords.x > 1.0 ||  projCoords.y < 0.0 || projCoords.y > 1.0)
          return 1.0; // fully lit if outside shadow map
        float closestDepth = texture(u_shadowMap, projCoords.xy).r;
        float currentDepth = projCoords.z;
        float bias = 0.002;
        return currentDepth - bias > closestDepth ? 0.5 : 1.0; // 0.5 shadow, 1.0 lit
    }

    void main()                                  
    {                

        vec3 base_color = texture(mesh_texture, tex_coord).rgb;
        vec3 normal = normalize(v_normal);  
        vec3 view_dir = normalize(u_view_position - frag_world_position);

        float world_distance = distance(u_view_position, frag_world_position);
        float fog_factor = smoothstep(0.0, 50.0, world_distance);

        float shadow = getShadow(frag_world_position);
        
        // ambient
        vec3 ambient_color = u_ambient_light.color * base_color;                           

        LightColorComponents directional_components = calculateLightComponents(
          u_directional_light.color,
          normalize(-u_directional_light.direction),
          view_dir,
          normal,
          base_color
        );

        LightColorComponents point_components = calculateLightComponents(
          u_point_light.color,
          normalize(u_point_light.position - frag_world_position),
          view_dir,
          normal,
          base_color
        );

        // point light attenuation
        float distance    = length(u_point_light.position - frag_world_position);
        float attenuation = 1.0 / (u_point_light.constant + u_point_light.linear * distance + u_point_light.quadratic * (distance * distance));  
        point_components.diffuse *= attenuation;
        
                                                                                                                                     
        outColor = vec4(ambient_color 
                      + directional_components.diffuse * shadow
                      + directional_components.specular * shadow
                      + point_components.diffuse / 1000.0 * shadow
                      + point_components.specular / 1000.0 * shadow, 
                      1.0); 

        outColor.rgb = min(outColor.rgb, vec3(1.0));
        outColor.rgb = mix(outColor.rgb, vec3(0.05, 0.05, 0.05), fog_factor);              
    }