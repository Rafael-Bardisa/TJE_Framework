
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_day_texture;
uniform sampler2D u_night_texture;
uniform sampler2D u_regions_texture;
uniform float u_time;
uniform vec3 u_sun_direction;

uniform vec4 u_selected_region_color;
//r -> region
//g -> government (country)
//b -> continent
uniform int u_region_type;
//1 -> country
//2 -> region
//3 -> continent
//bigger number -> bigger region


#define TOLERANCE 0.0001/255.0
#define PI 3.14


//calculate cool shader animation here for selected region
vec4 region_factor(vec4 u_selected_color, vec4 pixel_region_color){
    if (abs(u_selected_region_color.r - pixel_region_color.r) < TOLERANCE)
        return vec4(abs(sin(PI * u_time)));
    else
        return vec4(0);
}


void main()
{
	vec2 uv = v_uv;
    vec3 normal = normalize(v_normal);
    float night_factor = (clamp(dot(u_sun_direction, normal), -0.05, 0.05) + 0.05) * 10.0;
    vec4 tex_col_val = texture2D(u_day_texture, uv) * (1.0 - night_factor) + texture2D(u_night_texture, uv) * night_factor;
    
    
    
    // region render
    //TODO fill texture, pass color from mouse
    
    vec4 pixel_region_color = texture2D(u_regions_texture, uv);
    
    // checks before entering function are better because we don't change the context?
    if (u_selected_region_color.a >= 0.99 || pixel_region_color.a >= 0.99)
        tex_col_val += region_factor(u_selected_region_color, pixel_region_color);
    
	gl_FragColor = u_color * tex_col_val;
}
