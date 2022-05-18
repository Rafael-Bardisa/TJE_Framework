
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_day_texture;
uniform sampler2D u_night_texture;
uniform float u_time;
uniform vec3 u_sun_direction;

void main()
{
	vec2 uv = v_uv;
    vec3 normal = normalize(v_normal);
    float night_factor = (clamp(dot(u_sun_direction, normal), -0.05, 0.05) + 0.05) * 10.0;
    vec4 tex_col_val = texture2D(u_day_texture, uv) * (1.0 - night_factor) + texture2D(u_night_texture, uv) * night_factor;
	gl_FragColor = u_color * tex_col_val;
}
