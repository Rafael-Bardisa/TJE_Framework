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
    float day_factor = clamp(dot(u_sun_direction, normal), 0.0, 1.0);
    gl_FragColor = day_factor > 0.0 ? vec4(0, 1, 0, 1) : vec4 (0, 0, 1, 1);
}
