#version 330 core

in vec2 TexCoords;
out vec4 color;

const float offset = 1. / 300.;
vec2 offsets[9] = {
    {-offset, offset},
    {0.f, offset},
    {offset, offset},
    {-offset, 0.f},
    {0.f, 0.f},
    {offset, 0.f},
    {-offset, -offset},
    {0.f, -offset},
    {offset, -offset}
};
int edge_kernel[9] = {
    1, 1, 1,
    1, -8, 1,
    1, 1, 1
};
float blur_kernel[9] = {
    1./16, 2./16, 1./16,
    2./16, 4./16, 2./16,
    1./16, 2./16, 1./16
};

uniform sampler2D scene;

uniform bool chaos;
uniform bool confuse;
uniform bool shake;

void main() {
    color = vec4(0.f);
    vec3 sample[9];

    if (chaos || shake) {
        for (int i = 0; i < 9; i++) {
            sample[i] = vec3(texture(scene, TexCoords.xy + offsets[i]));
        }
    }

    if (chaos) {
        for (int i = 0; i < 9; i++) {
            color += vec4(sample[i] * edge_kernel[i], 0.f);
        }
        color.a = 1.f;
    } else if (confuse) {
        color = vec4(1. - texture(scene, TexCoords).rgb, 1.0);
    } else if (shake) {
        for (int i = 0; i < 9; i++) {
            color += vec4(sample[i] * blur_kernel[i], 0.f);
        }
        color.a = 1.f;
    } else {
        color = texture(scene, TexCoords);
    }

}
