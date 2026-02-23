vec3 toLinear(vec3 srgb) {
    return pow(srgb, vec3(2.2));
}
