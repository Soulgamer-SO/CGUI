#version 450

layout(location = 0) out vec3 vertex_color;

void main() {
    uint triangle = uint(gl_VertexIndex) / 3u;
    uint corner = uint(gl_VertexIndex) % 3u;
    float step_angle = 3.14159265359 / 5.0;
    uint point = triangle;
    uint next_point = (triangle + 1u) % 10u;
    float angle = -1.57079632679 + float(point) * step_angle;
    float next_angle = -1.57079632679 + float(next_point) * step_angle;
    float radius = (point % 2u == 0u) ? 0.78 : 0.34;
    float next_radius = (next_point % 2u == 0u) ? 0.78 : 0.34;
    vec2 point_position = vec2(cos(angle), sin(angle)) * radius;
    vec2 next_position = vec2(cos(next_angle), sin(next_angle)) * next_radius;
    vec2 center = vec2(0.0, 0.0);

    if(corner == 0u) {
        gl_Position = vec4(center, 0.0, 1.0);
    } else if(corner == 1u) {
        gl_Position = vec4(point_position, 0.0, 1.0);
    } else {
        gl_Position = vec4(next_position, 0.0, 1.0);
    }
    vertex_color = vec3(1.0, 0.15 + float(triangle) * 0.07, 0.05 + float(point % 2u) * 0.35);
}
