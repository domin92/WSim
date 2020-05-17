#version 330 core
#line 24
precision highp int;
precision highp float;

uniform sampler3D volume;
uniform ivec3 volume_dims;
uniform float dt_scale;

in vec3 vray_dir;
flat in vec3 transformed_eye;
out vec4 color;


vec2 intersect_box(vec3 orig, vec3 dir) {
    const vec3 box_min = vec3(0);
    const vec3 box_max = vec3(1);
    vec3 inv_dir = 1.0 / dir;
    vec3 tmin_tmp = (box_min - orig) * inv_dir;
    vec3 tmax_tmp = (box_max - orig) * inv_dir;
    vec3 tmin = min(tmin_tmp, tmax_tmp);
    vec3 tmax = max(tmin_tmp, tmax_tmp);
    float t0 = max(tmin.x, max(tmin.y, tmin.z));
    float t1 = min(tmax.x, min(tmax.y, tmax.z));
    return vec2(t0, t1);
}

// Pseudo-random number gen from
// http://www.reedbeta.com/blog/quick-and-easy-gpu-random-numbers-in-d3d11/
// with some tweaks for the range of values
float wang_hash(int seed) {
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return float(seed % 2147483647) / float(2147483647);
}

float linear_to_srgb(float x) {
    if (x <= 0.0031308f) {
        return 12.92f * x;
    }
    return 1.055f * pow(x, 1.f / 2.4f) - 0.055f;
}

void main(void) {
    float offset = wang_hash(int(gl_FragCoord.x + 640.0 * gl_FragCoord.y));
    float val = 1.2f;

    val = texture(volume, vec3(gl_FragCoord.x, gl_FragCoord.y, 1.0f)).r;
    color = vec4(0.2f, 0.2f, 0.2f, 0.0f);
    val = val * 100;
    if (val > 0.2f) {
        color = vec4(0.0f, 0.8f, 0.0f, 1.0f);
    }
    //vec4 val_color = vec4(texture(colormap, vec2(val, 0.5)).rgb, val);
    //color = texture(volume, vec3(0.5f, 0.5f, 0.5f));
}