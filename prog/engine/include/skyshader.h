#ifndef SKYSHADER_H
#define SKYSHADER_H

#include "shaderbase.h"
#include "mesh.h"
#include "camera.h"

class SkyShader : public ShaderBase
{
    public:
        SkyShader();
        virtual ~SkyShader();

        void init();

        void drawSkyQuad(   const Camera &cam_in,
                            const glm::vec4 &sky_color_in,
                            const glm::vec4 &fog_color_in   );
    protected:
    private:
        struct Uniforms
        {
            /// Matrices
            GLuint mvp_matrix;
            GLuint world_matrix;

            /// Colors
            GLuint sky_color;
            GLuint fog_color;

            /// Other parameters
            GLuint zfar;

        } uniforms;

        struct Attributes
        {
            GLuint vertex;

        } attributes;

        /// Sky quad geometry
        Mesh* sky_quad;
};

#endif // SKYSHADER_H
