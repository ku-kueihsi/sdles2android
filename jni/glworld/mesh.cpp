#include "mesh.h"
#include "image.h"
#include "data_utils.h"
#include "gl_tools.h"

using namespace std;
using namespace Eigen;
using namespace glw;

#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }
#define SAFE_CLEAR(v) if (v.size() > 0) { v.clear(); } 
#define MAX_BONE_WEIGHTS 3

void GLMatrix4fFromAssimp(GLMatrix4f &GLm, const aiMatrix4x4 &aim)
{
    GLm(0, 0) = aim.a1;
    GLm(0, 1) = aim.a2;
    GLm(0, 2) = aim.a3;
    GLm(0, 3) = aim.a4;
    GLm(1, 0) = aim.b1;
    GLm(1, 1) = aim.b2;
    GLm(1, 2) = aim.b3;
    GLm(1, 3) = aim.b4;
    GLm(2, 0) = aim.c1;
    GLm(2, 1) = aim.c2;
    GLm(2, 2) = aim.c3;
    GLm(2, 3) = aim.c4;
    GLm(3, 0) = aim.d1;
    GLm(3, 1) = aim.d2;
    GLm(3, 2) = aim.d3;
    GLm(3, 3) = aim.d4;
}

void GLMatrix3fFromAssimp(GLMatrix4f &GLm, const aiMatrix3x3 &aim)
{
    GLm(0, 0) = aim.a1;
    GLm(0, 1) = aim.a2;
    GLm(0, 2) = aim.a3;
    GLm(1, 0) = aim.b1;
    GLm(1, 1) = aim.b2;
    GLm(1, 2) = aim.b3;
    GLm(2, 0) = aim.c1;
    GLm(2, 1) = aim.c2;
    GLm(2, 2) = aim.c3;
}

void GLVector3fFromAssimp(GLVector3f &GLv, const aiVector3D &aiv)
{
    GLv(0) = aiv.x;
    GLv(1) = aiv.y;
    GLv(2) = aiv.z;
}

void GLQuaternionfFromAssimp(GLQuaternionf &GLq, const aiQuaternion &aiq)
{
    GLq.w() = aiq.w;
    GLq.x() = aiq.x;
    GLq.y() = aiq.y;
    GLq.z() = aiq.z;
}

ShaderProgram::ShaderProgram() :
mProgramId(0),
mVertexShaderId(0),
mFragmentShaderId(0)
{

}

ShaderProgram::ShaderProgram(const string& vertexStr, const string& fragmentStr):
mProgramId(0),
mVertexShaderId(0),
mFragmentShaderId(0)
{
    Load(vertexStr, fragmentStr);
}

ShaderProgram::~ShaderProgram()
{
    Clear();
}

void ShaderProgram::Clear()
{
    glDetachShader(mProgramId, mVertexShaderId);
    glDetachShader(mProgramId, mFragmentShaderId);
    glDeleteShader(mVertexShaderId);
    glDeleteShader(mFragmentShaderId);
    glDeleteProgram(mProgramId);

    mVertexShaderId = 0;
    mFragmentShaderId = 0;
    mProgramId = 0;
}

void ShaderProgram::Load(const std::string& vertexStr, const std::string& fragmentStr)
{
    //construct shader program form the char data block in the memory
    //
    //Embedded shader for test
    ////========================
    //  static const char *fragShaderText = "varying vec4 v_color;\n"
    //      "void main() {\n"
    //      "   gl_FragColor = v_color;\n"
    //      "}\n";
    //  static const char *vertShaderText = "uniform mat4 modelviewProjection;\n"
    //      "attribute vec4 pos;\n"
    //      "attribute vec4 color;\n"
    //      "varying vec4 v_color;\n"
    //      "void main() {\n"
    //      "   gl_Position = modelviewProjection * pos;\n"
    //      "   v_color = color;\n"
    //      "}\n";

    Clear();

    //    GLuint vertShader, fragShader;
    GLint stat;

    const char *kVertexShader = vertexStr.c_str();
    const char *kFragmentShader = fragmentStr.c_str();
    //  cout << VertexShader << endl << FragmentShader << endl;
    mVertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(mVertexShaderId, 1, (const char **) &kVertexShader, NULL);
    glCompileShader(mVertexShaderId);
    glGetShaderiv(mVertexShaderId, GL_COMPILE_STATUS, &stat);
    assert(stat);
    //    if (!stat) {
    //        printf("Error: vertex shader did not compile!\n");
    //        exit(1);
    //    }

    mFragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(mFragmentShaderId, 1, (const GLchar **) &kFragmentShader, NULL);
    glCompileShader(mFragmentShaderId);
    glGetShaderiv(mFragmentShaderId, GL_COMPILE_STATUS, &stat);
    assert(stat);
    //    if (!stat) {
    //        printf("Error: fragment shader did not compile!\n");
    //        exit(1);
    //    }

    mProgramId = glCreateProgram();
    glAttachShader(mProgramId, mVertexShaderId);
    glAttachShader(mProgramId, mFragmentShaderId);

    glLinkProgram(mProgramId);

    glGetProgramiv(mProgramId, GL_LINK_STATUS, &stat);
    assert(stat);
    //    if (!stat) {
    //        char log[1000];
    //        GLsizei len;
    //        glGetProgramInfoLog(program, 1000, &len, log);
    //        printf("Error: linking:\n%s\n", log);
    //        exit(1);
    //    }
}

void ShaderProgram::Use()
{
    assert(mProgramId);
    glUseProgram(mProgramId);
}

Texture::Texture(GLenum textureType) :
mTextureObjectId(0),
mTextureType(textureType)
{
}

Texture::Texture(const std::string& pngFileData, GLenum textureType)
{
    Load(pngFileData, textureType);
}

Texture::~Texture()
{
    Clear();
}

void Texture::Load(const std::string& pngFileData, GLenum textureType)
{
    Clear();

    string tmpstr;
    File2Str(tmpstr, pngFileData);
    if (tmpstr.size() > 1) {
        mTextureObjectId = load_png_memory_into_texture(tmpstr);
    } else {
        mTextureObjectId = load_png_memory_into_texture(pngFileData);
    }
    mTextureType = textureType;
    return;
}

void Texture::Clear()
{
    if (0 != mTextureObjectId) {
        glDeleteTextures(1, &mTextureObjectId);
    }
    mTextureType = GL_TEXTURE_2D;
    return;
}

void Texture::Bind(GLenum channel)
{
    assert(mTextureObjectId);
    glActiveTexture(channel);
    glBindTexture(mTextureType, mTextureObjectId);
}

void Mesh::Clear()
{
//    glDeleteBuffers(1, &mVbo);
//    glDeleteBuffers(1, &mIbo);
    glDeleteBuffers(NUM_DBs, mDataBuffers);
    mNumIndices = 0;
    mNumVertices = 0;
    mMaterialIndex = 0;
    return;
}

inline void FillDataBuffer(GLuint bufferIndex, const vector<GLfloat > &data, GLenum usage)
{
    assert(bufferIndex);
    glBindBuffer(GL_ARRAY_BUFFER, bufferIndex);
    glBufferData(GL_ARRAY_BUFFER, sizeof (GLfloat) * data.size(), data.data(), usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

inline void FillDataBuffer(GLuint bufferIndex, const vector<GLubyte > &data, GLenum usage)
{
    assert(bufferIndex);
    glBindBuffer(GL_ARRAY_BUFFER, bufferIndex);
    glBufferData(GL_ARRAY_BUFFER, sizeof (GLubyte) * data.size(), data.data(), usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void Mesh::Init(
        const std::vector<GLfloat>& positions,
        const std::vector<GLfloat>& normals,
        const std::vector<GLfloat>& textureCoordinates,
        const std::vector<GLubyte> &boneIds,
        const std::vector<GLfloat> &boneWeights,
        const std::vector<GLuint>& indices,
        GLuint materialIndex,
        GLenum usage
        )
{
    Clear();
    
    mNumVertices = positions.size() / 3;
    //generate vbos
    glGenBuffers(NUM_DBs, mDataBuffers);
//    //fill vertex buffer
//    assert(mDataBuffers[POSITION_DB]);
//    glBindBuffer(GL_ARRAY_BUFFER, mDataBuffers[POSITION_DB]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof (GLfloat) * positions.size(), positions.data(), usage);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
//    //fill normal buffer
//    assert(mDataBuffers[NORMAL_DB]);
//    assert(normals.size() / 3 == mNumVertices);
//    glBindBuffer(GL_ARRAY_BUFFER, mDataBuffers[NORMAL_DB]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof (GLfloat) * normals.size(), normals.data(), usage);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
//    //fill normal buffer
//    assert(mDataBuffers[TEXCOORD_DB]);
//    assert(textureCoordinates.size() / 2 == mNumVertices);
//    glBindBuffer(GL_ARRAY_BUFFER, mDataBuffers[TEXCOORD_DB]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof (GLfloat) * textureCoordinates.size(), textureCoordinates.data(), usage);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    FillDataBuffer(mDataBuffers[POSITION_DB], positions, usage);
    FillDataBuffer(mDataBuffers[NORMAL_DB], normals, usage);
    FillDataBuffer(mDataBuffers[TEXCOORD_DB], textureCoordinates, usage);
    FillDataBuffer(mDataBuffers[BONE_INDEX_DB], boneIds, usage);
    FillDataBuffer(mDataBuffers[BONE_WEIGHT_DB], boneWeights, usage);

    assert(mDataBuffers[INDEX_BUFFER]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mDataBuffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (GLuint) * indices.size(), indices.data(), usage);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    mNumIndices = indices.size();
    mMaterialIndex = materialIndex;
    
    return;
}

//void Mesh::Init(const std::vector<GLfloat > &positions,
//                const std::vector<GLfloat > &normals,
//                const std::vector<GLfloat > &textureCoordinates,
//                const std::vector<GLubyte > &boneIds,
//                const std::vector<GLfloat > &boneWeights,
//                const std::vector<GLuint > &indices, 
//                GLuint materialIndex, 
//                GLenum usage)
//{
//    Clear();
//    mNumVertices = positions.size() / 3;
//    //generate vbos
//    glGenBuffers(NUM_DBs, mDataBuffers);
//    //fill vertex buffer
//    assert(mDataBuffers[POSITION_DB]);
//    glBindBuffer(GL_ARRAY_BUFFER, mDataBuffers[POSITION_DB]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof (GLfloat) * positions.size(), positions.data(), usage);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    
//    //fill normal buffer
//    assert(mDataBuffers[NORMAL_DB]);
//    assert(normals.size() / 3 == mNumVertices);
//    glBindBuffer(GL_ARRAY_BUFFER, mDataBuffers[NORMAL_DB]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof (GLfloat) * normals.size(), normals.data(), usage);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    
//    //fill normal buffer
//    assert(mDataBuffers[TEXCOORD_DB]);
//    assert(textureCoordinates.size() / 2 == mNumVertices);
//    glBindBuffer(GL_ARRAY_BUFFER, mDataBuffers[TEXCOORD_DB]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof (GLfloat) * textureCoordinates.size(), textureCoordinates.data(), usage);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    
//    assert(mDataBuffers[INDEX_BUFFER]);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mDataBuffers[INDEX_BUFFER]);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (GLubyte) * indices.size(), indices.data(), usage);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//    
//    mNumIndices = indices.size();
//    mMaterialIndex = materialIndex;
//    
//    return;
//}

Mesh::Mesh(
        const std::vector<GLfloat> &positions,
        const std::vector<GLfloat> &normals,
        const std::vector<GLfloat> &textureCoordinates,
        const std::vector<GLubyte > &boneIds,
        const std::vector<GLfloat > &boneWeights,
        const std::vector<GLuint>& indices,
        GLuint materialIndex,
        GLenum usage
        ){
    memset(mDataBuffers, 0, sizeof(mDataBuffers));
    Init(positions, normals, textureCoordinates, boneIds, boneWeights, indices, materialIndex, usage);
}

Mesh::~Mesh()
{
    Clear();
}

Mesh::Mesh()
{
    memset(mDataBuffers, 0, sizeof(mDataBuffers));
}

void Mesh::Render()
{
    //explicit shader pointer, 0 for pos, 1 for tex, and 2 for normal
//    glEnableVertexAttribArray(0);
//    glEnableVertexAttribArray(1);
//    glEnableVertexAttribArray(2);
//    glEnableVertexAttribArray(3);
//    glEnableVertexAttribArray(4);

    for (int i = 0; i < NUM_DBs - 1; ++i){
        //index buffer counts one off
        glEnableVertexAttribArray(i);
    }

    //call core part
    DrawOn();

    for (int i = 0; i < NUM_DBs - 1; ++i){
        //index buffer counts one off
        glDisableVertexAttribArray(i);
    }

//    glDisableVertexAttribArray(4);
//    glDisableVertexAttribArray(3);
//    glDisableVertexAttribArray(2);
//    glDisableVertexAttribArray(1);
//    glDisableVertexAttribArray(0);

    return;
}

void Mesh::DrawOn()
{
    assert(mDataBuffers[POSITION_DB] && \
           mDataBuffers[NORMAL_DB] && \
           mDataBuffers[TEXCOORD_DB] && \
           mDataBuffers[BONE_INDEX_DB] && \
           mDataBuffers[BONE_WEIGHT_DB] && \
           mDataBuffers[INDEX_BUFFER]);
    
    glBindBuffer(GL_ARRAY_BUFFER, mDataBuffers[POSITION_DB]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, mDataBuffers[NORMAL_DB]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, mDataBuffers[TEXCOORD_DB]);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, mDataBuffers[BONE_INDEX_DB]);
    glVertexAttribPointer(3, MAX_BONE_WEIGHTS, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, mDataBuffers[BONE_WEIGHT_DB]);
    glVertexAttribPointer(4, MAX_BONE_WEIGHTS, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mDataBuffers[INDEX_BUFFER]);
    glDrawElements(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, 0);

    //unbind buffers
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return;
}

void RenderUnit::Clear()
{
    for (Mesh * pMesh : mMeshPtrList){
        SAFE_DELETE(pMesh);
    }
    SAFE_CLEAR(mMeshPtrList);
    
    for (Texture * pTexture : mTexturePtrList){
        SAFE_DELETE(pTexture);
    }
    SAFE_CLEAR(mTexturePtrList);
//    SAFE_CLEAR(mBoneInfoList);
    mAnimationTree.Clear();

    //SAFE_CLEAR(mBoneMap);
}

void RenderUnit::InitFromMemory(const std::string &fileData)
{
    Clear();
    Assimp::Importer importer;

    importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, MAX_BONE_WEIGHTS);
    //const aiScene* scene = Importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
    //const aiScene* scene = Importer.ReadFileFromMemory (fileData.c_str(), fileData.size(), aiProcess_JoinIdenticalVertices);
    
    //texture in opengl has a flipped order from the intuitive image storage. So far png and bmp are reported.
    //Instead load our texture in a reverse way, we flip uv in assimp
    const aiScene* pScene = importer.ReadFileFromMemory (fileData.c_str(), fileData.size(),
                aiProcess_Triangulate | 
                aiProcess_GenSmoothNormals | 
                aiProcess_JoinIdenticalVertices | 
                aiProcess_FlipUVs);  
    assert(pScene);
//    mpScene = pScene;

    mAnimationTree.Init(pScene);
    mInitTimeInSecond = std::chrono::high_resolution_clock::now();
    InitMeshes(pScene);
    InitTextures(pScene);
    return;
}

void RenderUnit::SetUpShader()
{
    for (unsigned int i = 0 ; i < mAnimationTree.mAction.animationList.size() ; ++i) {
        char Name[128];
        memset(Name, 0, sizeof(Name));
        sprintf(Name, "gBones[%d]", i);
        mBoneLocation[i] = glGetUniformLocation(mShaderIndex, Name);
    }

//    glGetUniformLocation(programobj.GetId(), "modelviewProjection");
    return;
}

void RenderUnit::InitAnimations(const aiScene* pScene)
{
//    assert(pScene);
//    //mMeshPtrList.resize(pScene->mNumMeshes);
//    for (int i = 0; i < pScene->mNumMeshes; ++i){
//        const aiMesh * pMesh = pScene->mMeshes[i];
////        assert(pMesh->HasBones());
//        for (int j = 0; j < pMesh->mNumBones; ++j){
//            string boneName(pMesh->mBones[i]->mName.data);
//            if (mBoneMap.find (boneName) != mBoneMap.end()){ //new bone to the map
//                GLubyte index = mBoneMap.size();
//                mBoneMap[boneName]=index;
//                //BoneInfo bi;
////                pMesh->mBones[0]->mWeights[0].mVertexId
////                mBoneInfoList
//            }
//        }
//    }
    return;
}

void RenderUnit::InitMeshes(const aiScene* pScene)
{
    assert(pScene);
    //mMeshPtrList.resize(pScene->mNumMeshes);
    for (int i = 0; i < pScene->mNumMeshes; ++i){
        const aiMesh * pMesh = pScene->mMeshes[i];
        //vector<GLfloat > vertexData(pMesh->mNumVertices * 8);
        vector<GLfloat > positions(pMesh->mNumVertices * 3);
        vector<GLfloat > normals(pMesh->mNumVertices * 3);
        vector<GLfloat > textureCoordinates(pMesh->mNumVertices * 2);
        vector<GLubyte > boneIds(pMesh->mNumVertices * MAX_BONE_WEIGHTS, 0);
        vector<GLfloat > boneWeights(pMesh->mNumVertices * MAX_BONE_WEIGHTS, 0);
        vector<GLubyte > boneCounts(pMesh->mNumVertices, 0);
        vector<GLuint > indexData(pMesh->mNumFaces * 3);


        assert(pMesh->HasPositions() && pMesh->HasNormals());

        for (int j = 0; j < pMesh->mNumVertices; ++j) {
            //cout << pMesh->mVertices[j].x << " " << pMesh->mVertices[j].y << " " << pMesh->mVertices[j].z << endl;
            //outf << "v " << pMesh->mVertices[j].x << " " << pMesh->mVertices[j].y << " " << pMesh->mVertices[j].z << "\n";

            //pos
            {
                int offset = j * 3;
                positions[offset] = pMesh->mVertices[j].x;
                positions[offset + 1] = pMesh->mVertices[j].y;
                positions[offset + 2] = pMesh->mVertices[j].z;
            }
            //normal
            {
                int offset = j * 3;
                normals[offset] = pMesh->mNormals[j].x;
                normals[offset + 1] = pMesh->mNormals[j].y;
                normals[offset + 2] = pMesh->mNormals[j].z;
            }
            //texture coordinate
            {
                int offset = j * 2;
                if (pMesh->HasTextureCoords(0)) {
                    textureCoordinates[offset] = pMesh->mTextureCoords[0][j].x;
                    textureCoordinates[offset + 1] = pMesh->mTextureCoords[0][j].y;
                } else {
//                    textureCoordinates[offset] = pMesh->mVertices[j].x;
//                    textureCoordinates[offset + 1] = pMesh->mVertices[j].y;
                    textureCoordinates[offset] = 0.0f;
                    textureCoordinates[offset + 1] = 0.0f;
                }
            }
        }

        //bone indices and weights
        for (int j = 0; j < pMesh->mNumBones; ++j){
            const aiBone *pBone = pMesh->mBones[j];
            string boneName(pBone->mName.data);
            GLubyte boneIndex = mAnimationTree.mAnimationMap[boneName];
            for (int k = 0; k < pBone->mNumWeights; ++k){
                const aiVertexWeight *pWeight = pBone->mWeights + k;
                if (boneCounts[pWeight->mVertexId] < MAX_BONE_WEIGHTS){
                    boneIds[pWeight->mVertexId * MAX_BONE_WEIGHTS+ boneCounts[pWeight->mVertexId]] = boneIndex;
                    boneWeights[pWeight->mVertexId * MAX_BONE_WEIGHTS + boneCounts[pWeight->mVertexId]] = pWeight->mWeight;
                    boneCounts[pWeight->mVertexId]++;
                }
            }
        }

        //normailize bone weights
//        for (int j = 0; j < pMesh->mNumVertices; ++j){
////            Matrix<GLfloat, MAX_BONE_WEIGHTS, 1, ColMajor> weightVector =
////                    Map<Matrix<GLfloat, MAX_BONE_WEIGHTS, 1, ColMajor> >(&(boneWeights[j * MAX_BONE_WEIGHTS]));

//            Map<Matrix<GLfloat, MAX_BONE_WEIGHTS, 1, ColMajor> > weightVector(&(boneWeights[0]) + (j * MAX_BONE_WEIGHTS));
//            weightVector.normalize();
//        }



        for (int j = 0; j < pMesh->mNumFaces; ++j){
            const aiFace& face = pMesh->mFaces[j];
            assert(face.mNumIndices == 3);
            int offset = j * 3;
            for (int k = 0; k < face.mNumIndices; ++k){
                indexData[offset + k] = face.mIndices[k];
            }
        }
        mMeshPtrList.push_back(new Mesh(positions, normals, textureCoordinates, boneIds, boneWeights, indexData, pMesh->mMaterialIndex));
        //meshobj.Init(vertexData, indexData);
    }
    return;
}

void RenderUnit::InitTextures(const aiScene* pScene)
{
    for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
        const aiMaterial* pMaterial = pScene->mMaterials[i];

//        m_Textures[i] = NULL;
        Texture *pTmpTexture = NULL;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                string fullPath = mResourcePath + "/" + path.data;
                pTmpTexture = new Texture(fullPath);
            }
            else{
                pTmpTexture = new Texture(mResourcePath + "/" + "dummy_white.png");
            }
        }
        else{
            pTmpTexture = new Texture(mResourcePath + "/" + "dummy_white.png");
        }
        mTexturePtrList.push_back(pTmpTexture);
    }
    if (0 == pScene->mNumMaterials){
        mTexturePtrList.push_back(new Texture(mResourcePath + "/" + "dummy_white.png"));
    }
    return;
}

void RenderUnit::DrawOn()
{
    //update bones
    mCurrentTimeInSecond = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> deltaTime = mCurrentTimeInSecond-mInitTimeInSecond;
    GLfloat time = fmod(deltaTime.count(), mAnimationTree.mAction.duration);
    mAnimationTree.UpdateAnimation(time);
    for (int i = 0; i < mAnimationTree.mAction.animationList.size(); ++i)
    {
//        GLMatrix4f testm = GLMatrix4f::Identity();
//        GLMatrix4f testm = mAnimationTree.mAction.animationList[i].finalTransformation;
//        glUniformMatrix4fv(mBoneLocation[i], 1, GL_FALSE, (const GLfloat*)testm.data());
        glUniformMatrix4fv(mBoneLocation[i], 1, GL_FALSE, (const GLfloat*)mAnimationTree.mAction.animationList[i].finalTransformation.data());
//        glUniformMatrix4fv(mBoneLocation[i], 1, GL_FALSE, (const GLfloat*)mAnimationTree.mAction.animationList[i].currentTransformation.data());
//        GLMatrix4f testm = GLMatrix4f::Identity();
//        scale_matrix(testm, (GLfloat(i) + 0.8f) * 3.0f);
//        glUniformMatrix4fv(mBoneLocation[i], 1, GL_FALSE, (const GLfloat* ) testm.data());
    }

    //Draw Meshes
    for (Mesh *pMesh : mMeshPtrList){
        if (pMesh->GetMaterialIndex() > mTexturePtrList.size() - 1){
            continue;
        }
        mTexturePtrList[pMesh->GetMaterialIndex()]->Bind();
        pMesh->DrawOn();
    }
    return;
}

void RenderUnit::Render()
{
    //explicit shader pointer, 0 for pos, 1 for tex, and 2 for normal
    for (int i = 0; i < NUM_DBs - 1; ++i){
        //index buffer counts one off
        glEnableVertexAttribArray(i);
    }

    //call core part
    DrawOn();

    for (int i = 0; i < NUM_DBs - 1; ++i){
        //index buffer counts one off
        glDisableVertexAttribArray(i);
    }
   
    return;
}

void RenderUnit::setResourcePath(string path){
	mResourcePath = path;
}

RenderUnit::~RenderUnit()
{
    Clear();
}

RenderUnit::RenderUnit():
mResourcePath("./")
{
}

//BoneAnimation::~BoneAnimation()
//{
//    Clear();
//}

//void BoneAnimation::Clear()
//{
////    for(BoneAnimation *pBone : mpChildren)
////    {
////        SAFE_DELETE(pBone);
////    }
////    SAFE_CLEAR(mpChildren);
//    SAFE_CLEAR(mTranslationSamples);
//    SAFE_CLEAR(mQuaternionSamples);
//    SAFE_CLEAR(mScaleSamples);
//    mBoneName = string("");
//    mParentIndex = 0;
//    mCurrentTick = 0;
//    return;
//}

//void BoneAnimation::Init(
//        const string &name,
//        const GLMatrix4f &baseTransformation,
//        const std::vector<TranslationSample> &translationSamples,
//        const std::vector<QuaternionSample> &quaternionSamples,
//        const std::vector<ScaleSample> &scaleSamples
////        const std::vector<Bone *> &pChildren
//        )
//{
//    Clear();
//    mBoneName = name;
//    mBaseTransformation = baseTransformation;
//    mTranslationSamples = translationSamples;
//    mQuaternionSamples = quaternionSamples;
//    mScaleSamples = scaleSamples;
////    mpChildren = pChildren;
//    return ;
//}

//BoneAnimation::BoneAnimation(
//        const std::string &name, \
//        const GLMatrix4f &baseTransformation, \
//        const std::vector<TranslationSample > &translationSamples, \
//        const std::vector<QuaternionSample > &quaternionSamples, \
//        const std::vector<ScaleSample > &scaleSamples
//        )
//        //         const std::vector<Bone * > &pChildren \
////        )
//{
//    Init(name, baseTransformation, translationSamples, quaternionSamples, scaleSamples);
//}

//BoneAnimation::BoneAnimation()
//{
//    Clear();
//}

//Skeleton::~Skeleton()
//{
//    Clear();
//}

//void Skeleton::Clear()
//{
//    SAFE_CLEAR(mBoneAnimations);
//    SAFE_CLEAR(mBoneMap);
//}

//void Skeleton::Init(const aiScene *pScene)
//{
//    assert(pScene);
//    assert(pScene->HasAnimations());
//    Clear();
//    //serialize the node tree, build bone map
//    //preoder traverse guarantees parents are visted first

//    //number of nodes == number of animation channels
//    mBoneAnimations.resize(pScene->mAnimations[0]->mNumChannels);
//    cout << "size " << mBoneAnimations.size() << endl;
//    GLubyte baseIndex = 0;
//    BuildAnimationListRecursive(pScene->mRootNode, baseIndex);


//    //fill animations

//    return;
//}

//void Skeleton::BuildAnimationListRecursive(const aiNode *pNode, GLuint &baseIndex)
//{
//    //assert(pNode && baseIndex < mBoneAnimations.size());
//    cout << "baseIndex " << baseIndex << endl;
//    string nodeName = string(pNode->mName.data);
////    if (mBoneMap.find(nodeName) == mBoneMap.end()){
////        mBoneMap[nodeName] = baseIndex;
////        mBoneAnimations[baseIndex].mBoneName = nodeName;
////        ++baseIndex;
////    }
//    cout << nodeName << endl;
//    for (int i =0; i < pNode->mNumChildren; ++i){
//        const aiNode *pChildNode = pNode->mChildren[i];
//        BuildAnimationListRecursive(pChildNode, baseIndex);
//    }
//    return;
//}

void NodeAnimation::InterpolateTransformation(GLfloat time)
{
    //interpolate translation
    GLMatrix4f translationMatrix(GLMatrix4f::Identity());
    if (time < translationSamples.back().time)
    {
        if (translationSamples[translationTick].time > time){
            //rewind
            translationTick = 0;
        }
        while(translationSamples[translationTick + 1].time < time){
            translationTick++;
        }
        translationTick %= translationSamples.size() - 1;
        GLfloat t1 = translationSamples[translationTick].time;
        GLfloat t2 = translationSamples[translationTick + 1].time;
        GLVector3f interVector =
                translationSamples[translationTick].translation * ((time - t1) / (t2 - t1))
                +
                translationSamples[translationTick + 1].translation * ((t2 - time) / (t2 - t1));
        for (int i = 0; i < 3; ++i){
            translationMatrix(i, 3) = interVector(i);
        }
    }

    //interpolate quaternion
    GLMatrix4f rotationMatrix(GLMatrix4f::Identity());
    if (time < quaternionSamples.back().time)
    {
        if (quaternionSamples[quaternionTick].time > time){
            //rewind
            quaternionTick = 0;
        }
        while(quaternionSamples[quaternionTick + 1].time < time){
            quaternionTick++;
        }
        quaternionTick %= quaternionSamples.size() - 1;
        GLfloat t1 = quaternionSamples[quaternionTick].time;
        GLfloat t2 = quaternionSamples[quaternionTick + 1].time;

        GLQuaternionf interQuaterion =
                quaternionSamples[quaternionTick].quaternion.slerp(
                    ((time - t1) / (t2 - t1)),
                    quaternionSamples[quaternionTick + 1].quaternion
                );
        interQuaterion.normalize();
        for (int i = 0; i < 3; ++i){
            rotationMatrix.block<3, 3 >(0, 0) = interQuaterion.toRotationMatrix();
        }
    }

    //interpolate scale
    GLMatrix4f scaleMatrix(GLMatrix4f::Identity());
    if (time < scaleSamples.back().time)
    {
        if (scaleSamples[scaleTick].time > time){
            //rewind
            scaleTick = 0;
        }
        while(scaleSamples[scaleTick + 1].time < time){
            scaleTick++;
        }
        scaleTick %= scaleSamples.size() - 1;
        GLfloat t1 = scaleSamples[scaleTick].time;
        GLfloat t2 = scaleSamples[scaleTick + 1].time;
        GLVector3f interVector =
                scaleSamples[scaleTick].scale * ((time - t1) / (t2 - t1))
                +
                scaleSamples[scaleTick + 1].scale * ((t2 - time) / (t2 - t1));
        for (int i = 0; i < 3; ++i){
            scaleMatrix(i, i) = interVector(i);
        }
    }

    currentTransformation = translationMatrix * rotationMatrix * scaleMatrix;

    return;
}

UnitNode::~UnitNode()
{
    for (UnitNode *pNode : mChildren){
        SAFE_DELETE(pNode);
    }
    SAFE_CLEAR(mChildren);
}

UnitNode::UnitNode()
{
    mName = string("");
    mHasNodeAnimation = false;
    mAnimationIndex = 255;
    mTransformation = GLMatrix4f::Identity();
    SAFE_CLEAR(mChildren);
}

UnitTree::UnitTree():
mRootNode(NULL)
{ 
    Clear();
}

void UnitTree::Clear()
{
    SAFE_CLEAR(mAnimationMap);
    SAFE_CLEAR(mAction.animationList);
    SAFE_DELETE(mRootNode);
    mActionIndex = 0;
    mGlobalInverseTransform = GLMatrix4f::Identity();
}

UnitTree::~UnitTree()
{
    Clear();
//    SAFE_CLEAR(mAnimationMap);
//    SAFE_DELETE(mRootNode);
}

void UnitTree::BuildFromAssimpRecursive(UnitNode *&pNode, const aiNode *pAiNode)
{
    assert(pAiNode);
    pNode = new UnitNode;
    pNode->mName = string(pAiNode->mName.data);
    if (mAnimationMap.find(pNode->mName) != mAnimationMap.end()){
        pNode->mAnimationIndex = mAnimationMap[pNode->mName];
        pNode->mHasNodeAnimation = true;
    }else{
        pNode->mAnimationIndex = 255;
        pNode->mHasNodeAnimation = false;
    }
    //pNode->mTransformation = pAiNode->mTransformation
    GLMatrix4fFromAssimp(pNode->mTransformation, pAiNode->mTransformation);
    pNode->mChildren.reserve(pAiNode->mNumChildren);
    for (int i = 0; i < pAiNode->mNumChildren; ++i){
        UnitNode *tmpNode = NULL;
        BuildFromAssimpRecursive(tmpNode, pAiNode->mChildren[i]);
        pNode->mChildren.push_back(tmpNode);
    }
    return;
}

void UnitTree::Init(const aiScene *pScene)
{
    assert(pScene);
    Clear();
    GLMatrix4fFromAssimp(mGlobalInverseTransform, pScene->mRootNode->mTransformation.Inverse());
//    GLMatrix4fFromAssimp(mGlobalInverseTransform, pScene->mRootNode->mTransformation);
    mGlobalInverseTransform = mGlobalInverseTransform.inverse().eval();
    InitAction(pScene);
    BuildFromAssimpRecursive(mRootNode, pScene->mRootNode);
}

void UnitTree::InitAction(const aiScene *pScene){
    assert(pScene->HasAnimations());
    const aiAnimation *pAnimation = pScene->mAnimations[0];
    mAction.name = string(pAnimation->mName.data);
    if (pAnimation->mTicksPerSecond == 0){
        mAction.timePerTick = 1.0f / 25.0f;
    }else{
        mAction.timePerTick = 1.0f / GLfloat(pAnimation->mTicksPerSecond);
    }
    mAction.duration = pAnimation->mDuration * mAction.timePerTick;
    mAction.animationList.reserve(pAnimation->mNumChannels);
    for (int i = 0; i < pAnimation->mNumChannels; ++i){
        const aiNodeAnim *pNodeAnim = pAnimation->mChannels[i];
        string nodeName(pNodeAnim->mNodeName.data);
        mAnimationMap[nodeName] = i;
        //fill translations
//        mAction.animationList[i].translationSamples.resize(pNodeAnim->mNumPositionKeys);
        vector<TranslationSample > translationSamples;
        for (int j = 0; j < pNodeAnim->mNumPositionKeys; ++j){
            const aiVectorKey *pKey = pNodeAnim->mPositionKeys + j;
            TranslationSample sample;
            sample.time = pKey->mTime * mAction.timePerTick;
            GLVector3fFromAssimp(sample.translation, pKey->mValue);
            translationSamples.push_back(sample);
        }

        //file quaternions
        vector<QuaternionSample > quaternionSamples;
        for (int j = 0; j < pNodeAnim->mNumRotationKeys; ++j){
            const aiQuatKey *pKey = pNodeAnim->mRotationKeys + j;
            QuaternionSample sample;
            sample.time = pKey->mTime * mAction.timePerTick;
            GLQuaternionfFromAssimp(sample.quaternion, pKey->mValue);
            quaternionSamples.push_back(sample);
        }

        //file scale
        vector<ScaleSample > scaleSamples;
        for (int j = 0; j < pNodeAnim->mNumScalingKeys; ++j){
            const aiVectorKey *pKey = pNodeAnim->mScalingKeys + j;
            ScaleSample sample;
            sample.time = pKey->mTime * mAction.timePerTick;
            GLVector3fFromAssimp(sample.scale, pKey->mValue);
            scaleSamples.push_back(sample);
        }
        NodeAnimation animation = {
            nodeName,
            translationSamples,
            quaternionSamples,
            scaleSamples,
            GLMatrix4f::Identity(),
            GLMatrix4f::Identity(),
            GLMatrix4f::Identity(),
            0,
            0,
            0
        };
        mAction.animationList.push_back(animation);
    }

    //init offset transformations
    unordered_map<string, bool> tmpBoneMap;
    for (int i = 0; i < pScene->mNumMeshes; ++i){
        const aiMesh *pMesh = pScene->mMeshes[i];
        for (int j = 0; j < pMesh->mNumBones; ++j){
            const aiBone *pBone = pMesh->mBones[j];
            string boneName(pBone->mName.data);
            if (tmpBoneMap.find(boneName) == tmpBoneMap.end()){
                tmpBoneMap[boneName] = true;
                GLubyte index = mAnimationMap[boneName];
                GLMatrix4fFromAssimp(mAction.animationList[index].offsetTransformation, pBone->mOffsetMatrix);
            }
        }
    }
}

void UnitTree::UpdateAnimation(GLfloat time)
{
    GLfloat interpolationTime = fmod(time, mAction.duration);
    for (NodeAnimation &na : mAction.animationList){
        na.InterpolateTransformation(interpolationTime);
    }
//    UpdateAnimationRecursive(mRootNode, GLMatrix4f::Identity(), fmod(time, mAction.duration));
    UpdateAnimationRecursive(mRootNode, mGlobalInverseTransform, fmod(time, mAction.duration));
    return;
}

void UnitTree::UpdateAnimationRecursive(UnitNode *pNode, const GLMatrix4f &parentTransformation, GLfloat time)
{
    GLMatrix4f globalTransformation = parentTransformation * pNode->mTransformation;
    if (pNode->mHasNodeAnimation){
        GLubyte boneIndex = mAnimationMap[pNode->mName];
        //mAction.animationList[boneIndex].UpdateTransformation(time);
//        if (1 == boneIndex){
//            globalTransformation = parentTransformation;
//        }else{
//            globalTransformation = parentTransformation * mAction.animationList[boneIndex].currentTransformation;
//        }
        globalTransformation = parentTransformation * mAction.animationList[boneIndex].currentTransformation;
//        mAction.animationList[boneIndex].finalTransformation = globalTransformation;
//        mAction.animationList[boneIndex].finalTransformation =
//                mGlobalInverseTransform * globalTransformation * mAction.animationList[boneIndex].offsetTransformation;
        mAction.animationList[boneIndex].finalTransformation =
                        globalTransformation * mAction.animationList[boneIndex].offsetTransformation;
    }
    for (UnitNode *pChild : pNode->mChildren) {
        UpdateAnimationRecursive(pChild, globalTransformation, time);
    }


    return;
}

void UnitTree::Print()
{
    int depth = 0;
    //print animations
    cout << "duration: " << mAction.duration << endl;
    cout << "tick: " << mAction.timePerTick << endl;
    //print nodes
    PrintRecursive(mRootNode, depth);

    for (NodeAnimation &anim : mAction.animationList){
        cout << anim.nodeName << endl;
        cout << "translation:" << endl;
        for (TranslationSample &ts : anim.translationSamples){
            cout << ts.time << endl << ts.translation.transpose() << endl;
        }
        cout << "quaternion:" << endl;
        for (QuaternionSample &qs : anim.quaternionSamples){
            cout << qs.time << endl << qs.quaternion.normalized().w() << "," << qs.quaternion.normalized().vec().transpose() << endl;
        }
        cout << "scale:" << endl;
        for (ScaleSample &ss : anim.scaleSamples){
            cout << ss.time << endl << ss.scale.transpose() << endl;
        }
//        cout << anim.currentTransformation << endl;
        cout << "current: " << endl;
        for (int i = 0; i < anim.currentTransformation.size(); ++i){
            cout << anim.currentTransformation.data()[i] << ", ";
        }
        cout << endl;
        cout << "offset: " << endl;
        for (int i = 0; i < anim.offsetTransformation.size(); ++i){
            cout << anim.offsetTransformation.data()[i] << ", ";
        }
        cout << endl;
        cout << "final: " << endl;
        for (int i = 0; i < anim.offsetTransformation.size(); ++i){
            cout << anim.finalTransformation.data()[i] << ", ";
        }
        cout << endl;
    }
    cout << endl;
}

void UnitTree::PrintRecursive(const UnitNode *pNode, int depth)
{
    cout << string(depth, '>') << pNode->mName << endl;
    cout << string(depth, '>') << GLubyte(pNode->mAnimationIndex) << endl;
//    cout << pNode->mTransformation << endl;
    for (int i = 0; i < pNode->mTransformation.size(); ++i){
        cout << pNode->mTransformation.data()[i] << ", ";
    }
    cout << endl;
    for (const UnitNode *pChild : pNode->mChildren){
        PrintRecursive(pChild, depth + 1);
    }
}
