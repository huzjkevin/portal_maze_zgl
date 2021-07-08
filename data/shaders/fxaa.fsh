uniform sampler2DRect uTexColor;
//uniform uvec2 uScreenSize;

out vec3 fColor;

//vec3 FXAA1();
vec3 FXAA();
float computeLuma(vec3 color);
//vec4 computeEdgeUV(vec2 uv, vec2 offset, float luma, float grad);
//float stepSize(int iter);

void main()
{
    //vec3 color = texture(uTexColor, gl_FragCoord.xy).rgb;
    //vec3 color = FXAA1();
    vec3 color = FXAA();

    // conversion to sRGB
    fColor = pow(color, vec3(1 / 2.224));
}

//This structure and formula used are refferred to book "OpenGL ES 3.0 Cookbook, page 405 - 411", by Parminder Singh
vec3 FXAA(){    
    vec2 uv = gl_FragCoord.xy;
    //Some parameters introduced in the algorithm
    float FXAA_SPAN_MAX = 8.0;
    float FXAA_REDUCE_MUL = 1.0/8.0;
    float FXAA_REDUCE_MIN = 1.0/128.0;
    float thrMin = 0.0312;
    float thrMax = 0.125;

    //Luma for 5 samples
    vec3 colorC = texture(uTexColor, uv).rgb;
    vec3 colorNW = textureOffset(uTexColor, uv, ivec2(-1, -1)).rgb;
    vec3 colorNE = textureOffset(uTexColor, uv, ivec2(1, -1)).rgb;
    vec3 colorSW = textureOffset(uTexColor, uv, ivec2(-1, 1)).rgb;
    vec3 colorSE = textureOffset(uTexColor, uv, ivec2(1, 1)).rgb;

    float lumaC = computeLuma(colorC);
    float lumaNW = computeLuma(colorNW);
    float lumaNE = computeLuma(colorNE);
    float lumaSW = computeLuma(colorSW);
    float lumaSE = computeLuma(colorSE);

    float lumaMin = min(lumaC, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaC, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    float absDiff = lumaMax - lumaMin;

    //Edge detection, using settings used in FXAA1()
    if(absDiff < max(thrMin, lumaMax * thrMax)){
        //Do nothing
        return texture(uTexColor, gl_FragCoord.xy).rgb;
    } else {

        //Considered as edge fragment
        //formula for determining edge orientation and unit length of direction vector
        vec2 edgeDir = vec2((lumaSW + lumaSE) - (lumaNW + lumaNE), (lumaNW + lumaSW) - (lumaNE + lumaSE));
        float dirReduce =  max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN); //Reduced direction introduuced in the book
        float vectorUnit = 1.0 / (min(abs(edgeDir.x), abs(edgeDir.y)) + dirReduce);

        //Compute length of direction vector
        edgeDir = clamp(edgeDir * vectorUnit, vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX));

        //formula for taking 2/4 samples along the direction
        vec3 sample2 = 0.5 * (texture(uTexColor, uv + edgeDir / 6.0).rgb + texture(uTexColor, uv - edgeDir / 6.0).rgb);
        vec3 sample4 = 0.25 * (texture(uTexColor, uv + 0.5 * edgeDir).rgb + texture(uTexColor, uv - 0.5 * edgeDir).rgb) + 0.5 * sample2;
        float lumaSample4 = computeLuma(sample4);

        //Determine whether sample is beyond the edge
        if((lumaSample4 < lumaMin) || (lumaSample4 > lumaMax)){
            return sample2;
        } else {
            return sample4;
        }
    }
}






/*
//TODO: This method does not function correctly.
vec3 FXAA1(){
    //Some formulas referred at: http://blog.simonrodriguez.fr/articles/30-07-2016_implementing_fxaa.html
    vec2 uv = gl_FragCoord.xy;
    //vec2 uv = vec2(gl_FragCoord.x / float(uScreenSize.x), gl_FragCoord.y / float(uScreenSize.y));
    float thrMin = 0.0312;
    float thrMax = 0.125;
    float grad = 0.0;
    float unitX = 1.0;
    float unitY = 1.0;
    //float unitX = 1.0;
    //float unitY = 1.0;
    float averageLuma = 0.0;
    vec2 offset = vec2(0.0, 0.0);
    float pixelOffset = 0.0;
    float subpixelQuality = 0.75;

    //convert color into luma for detecting edge 

    float center = computeLuma(texture(uTexColor, uv).rgb);
    float left = computeLuma(textureOffset(uTexColor, uv, ivec2(-1, 0)).rgb);
    float right = computeLuma(textureOffset(uTexColor, uv, ivec2(1, 0)).rgb);
    float up = computeLuma(textureOffset(uTexColor, uv, ivec2(0, 1)).rgb);
    float down = computeLuma(textureOffset(uTexColor, uv, ivec2(0, -1)).rgb);
    float upLeft = computeLuma(textureOffset(uTexColor, uv, ivec2(-1, 1)).rgb);
    float upRight = computeLuma(textureOffset(uTexColor, uv, ivec2(1, 1)).rgb);
    float downLeft = computeLuma(textureOffset(uTexColor, uv, ivec2(-1, -1)).rgb);
    float downRight = computeLuma(textureOffset(uTexColor, uv, ivec2(1, -1)).rgb);

    float lumaMax = max(center, max(left, max(right, max(up, down))));
    float lumaMin = min(center, min(left, min(right, min(up, down))));

    float absDiff = lumaMax - lumaMin;

    //Recommended threshold setting
    if(absDiff < max(thrMin, lumaMax * thrMax)){
        //Do nothing
        return texture(uTexColor, gl_FragCoord.xy).rgb;
    } else {
        //Do FXAA
        //edge detection, using formula in the article
        float diffX = abs((upLeft - left) - (left - downLeft)) + 2 * abs((up - center) - (center - down)) + abs((upRight - right) - (right - downRight));
        float diffY = abs((upRight - up) - (up - upLeft)) + 2 * abs((right - center) - (center - left)) + abs((downRight - down) - (down - downLeft));
        
        
        if(diffX > diffY) {
            //return vec3(1.0, 1.0, 1.0);
            //gradient is along Y direction

            float gradUp = up - center;
            float gradDown = down - center;
            grad = max(abs(gradUp), abs(gradDown));
            //grad = max(gradUp, gradDown);
            offset = vec2(unitX, 0.0);
            
            //Using formula in the article to normalize gradient
            grad = grad * 0.25;

            //Compute average luma at point shifted half-unit towards gradient direction
            if (gradUp > gradDown) {
                averageLuma = 0.5 * (up + center);
                uv.y = uv.y + 0.5 * unitY;
                //if(uv.y == gl_FragCoord.y){return vec3(1.0, 1.0, 1.0);}
            } else {
                averageLuma = 0.5 * (down + center);
                uv.y = uv.y - 0.5 * unitY;

            }

            //vec2 uvLeft = uv - offset;
            //vec2 uvRight = uv + offset;

            vec4 edgeUV = computeEdgeUV(uv, offset, averageLuma, grad);
            
            float lumaPos = computeLuma(texture(uTexColor, edgeUV.xy).rgb);
            float lumaNeg = computeLuma(texture(uTexColor, edgeUV.yw).rgb);
            vec2 distPos = edgeUV.xy - uv;
            vec2 distNeg = uv - edgeUV.yw;   
            float dist = min(distPos.x, distNeg.x);
            float edgeLength = distPos.x + distNeg.x;
            pixelOffset = - dist / edgeLength + 0.5; //formula in the article

            float lumaAverage = (up + down + left + right) / 4.0;
            float subPixelOffset1 = clamp(abs(lumaAverage - center)/absDiff, 0.0, 1.0);
            float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;
            float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * subpixelQuality;
            pixelOffset = max(pixelOffset, subPixelOffsetFinal);

            vec2 uv_AA = gl_FragCoord.xy;
            //vec2 uv_AA = vec2(gl_FragCoord.x / float(uScreenSize.x), gl_FragCoord.y / float(uScreenSize.y));
            uv_AA.y += pixelOffset * unitY;
            
            //if (equal( uv_AA, gl_FragCoord.xy).x && equal(uv_AA,  gl_FragCoord.xy).y){return vec3(1.0, 0.0, 0.0);}
            return texture(uTexColor, uv_AA).rgb;  
            //return (1.0 - 0.5) * texture(uTexColor, uv_AA + vec2(1, 1)).rgb + 0.5 * texture(uTexColor, gl_FragCoord.xy).rgb;

        } else {
            //return vec3(1.0, 1.0, 1.0);
            //gradient is along X direction

            float gradLeft = left - center;
            float gradRight = right - center;
            grad = max(abs(gradLeft), abs(gradRight));
            offset = vec2(0.0, unitY);

            //Using formula in the article to normalize gradient
            grad = grad * 0.25;

            //Compute average luma at point shifted half-unit towards gradient direction
            if (gradLeft > gradRight) {
                averageLuma = 0.5 * (left + center);
                uv.x = uv.x - 0.5 * unitX;
            } else {
                averageLuma = 0.5 * (right + center);
                uv.x = uv.x + 0.5 * unitX;
            }
            //vec2 uvUp = uv + offset;
            //vec2 uvDown = uv - offset;

            vec4 edgeUV = computeEdgeUV(uv, offset, averageLuma, grad);
            float lumaPos = computeLuma(texture(uTexColor, edgeUV.xy).rgb);
            float lumaNeg = computeLuma(texture(uTexColor, edgeUV.yw).rgb);
            vec2 distPos = edgeUV.xy - uv;
            vec2 distNeg = uv - edgeUV.yw;   
            
            float dist = min(distPos.y, distNeg.y);
            float edgeLength = distPos.y + distNeg.y;
            pixelOffset = - dist / edgeLength + 0.5; //formula in the article

            float lumaAverage = (1.0/12.0) * (2.0 * (down + up + left + right) + downLeft + upLeft + downRight + upRight);
            float subPixelOffset1 = clamp(abs(lumaAverage - center)/absDiff, 0.0, 1.0);
            float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;
            float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * subpixelQuality;
            pixelOffset = max(pixelOffset, subPixelOffsetFinal);

            vec2 uv_AA = gl_FragCoord.xy;
            //vec2 uv_AA = vec2(gl_FragCoord.x / float(uScreenSize.x), gl_FragCoord.y / float(uScreenSize.y));
            uv_AA.x += pixelOffset * unitX;
            //return 10000*vec3(uv_AA - gl_FragCoord.xy, 0.0);
            //if (equal(100000 * uv_AA, 100000 * gl_FragCoord.xy).x && equal(100000 * uv_AA, 100000 * gl_FragCoord.xy).y){return vec3(1.0, 0.0, 0.0);}
            return texture(uTexColor, uv_AA).rgb;
            //return (1.0 - pixelOffset) * texture(uTexColor, uv_AA).rgb + pixelOffset * texture(uTexColor, gl_FragCoord.xy).rgb;
        } 
    }   
}
*/

float computeLuma(vec3 color){
    return dot(color, vec3(0.299, 0.587, 0.114));
}

/*
vec4 computeEdgeUV(vec2 uv, vec2 offset, float luma, float grad) {
    vec2 uvPos = uv + offset;
    vec2 uvNeg = uv - offset;
    int iter = 1;
    
    float lumaPos = computeLuma(texture(uTexColor, uvPos).rgb);
    float lumaNeg = computeLuma(texture(uTexColor, uvNeg).rgb);
    
    float DiffPos = lumaPos - luma;
    float DiffNeg = lumaNeg - luma;
    
    bool isEndPos = abs(DiffPos) >= grad;
    bool isEndNeg = abs(DiffNeg) >= grad;
    
    while(!(isEndPos && isEndNeg)){
        if(!isEndPos) {
            uvPos += offset * stepSize(iter);
            lumaPos = computeLuma(texture(uTexColor, uvPos).rgb);
            DiffPos = lumaPos - luma;
            isEndPos = abs(DiffPos) >= grad; //Edge detection
        }
        if(!isEndNeg) {
            uvNeg -= offset * stepSize(iter);
            lumaNeg = computeLuma(texture(uTexColor, uvNeg).rgb);
            DiffNeg = lumaNeg - luma;
            isEndNeg = abs(DiffNeg) >= grad; //Edge detection
        }
        iter++;
        if(iter >= 12){break;}
    }
    
    vec4 edgeUV = vec4(uvPos, uvNeg);

    return edgeUV;
}

float stepSize(int iter){
    if(iter < 5) {
        return 1.0;
    } else if (iter < 10) {
        return 2.0;
    } else {
        return 4.0;
    }
}
*/
