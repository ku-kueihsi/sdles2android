
#ifndef __MESH_H__
#define	__MESH_H__

#include <assimp/Importer.hpp>
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags
#include <eigen3/Eigen/Dense>
#include "platform_gl.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>


namespace glw {
//using namespace std;
//using namespace Eigen;
#define MAX_BONES 100

typedef Eigen::Matrix<GLfloat, 4, 4, Eigen::ColMajor > GLMatrix4f;
typedef Eigen::Matrix<GLfloat, 3, 3, Eigen::ColMajor > GLMatrix3f;
typedef Eigen::Matrix<GLfloat, 3, 1, Eigen::ColMajor > GLVector3f;
typedef Eigen::Quaternion<GLfloat > GLQuaternionf;

enum DB_TYPES {
    INDEX_BUFFER,
    POSITION_DB,
    NORMAL_DB,
    TEXCOORD_DB,
    BONE_INDEX_DB,
    BONE_WEIGHT_DB,
    NUM_DBs            
};

class ShaderProgram{
public:
    ShaderProgram();
    ShaderProgram(const std::string &vertexStr, const std::string &fragmentStr);
    virtual ~ShaderProgram();
    void Clear();
    void Load(const std::string &vertexStr, const std::string &fragmentStr);
    void Use();
    GLuint GetId(){return mProgramId;}
    
private:
    GLuint mProgramId;
    GLuint mVertexShaderId;
    GLuint mFragmentShaderId;
    
};

class Texture {
    //texture encapsulation
    //supports single 2D texture so far
    //
public:
    Texture(GLenum textureType = GL_TEXTURE_2D);
    Texture(const std::string &pngFileData, GLenum textureType = GL_TEXTURE_2D);
    virtual ~Texture();
    void Bind(GLenum channel = GL_TEXTURE0);
    void Load(const std::string &pngFileData, GLenum textureType = GL_TEXTURE_2D);
    void Clear();
    
private:
   GLuint mTextureObjectId; 
   GLenum mTextureType;
//   GLenum mTexutreChannel;
};

//class BoneAnimation
//{
//public:
//    BoneAnimation();
//    BoneAnimation(const std::string &name, \
//         const GLMatrix4f &baseTransformation, \
//         const std::vector<TranslationSample> &translationSamples, \
//         const std::vector<QuaternionSample> &quaternionSamples, \
//         const std::vector<ScaleSample> &scaleSamples);
//    virtual ~BoneAnimation();
//    void Init(const std::string &name, \
//            const GLMatrix4f &baseTransformation, \
//            const std::vector<TranslationSample> &translationSamples, \
//            const std::vector<QuaternionSample> &quaternionSamples, \
//            const std::vector<ScaleSample> &scaleSamples);
//    void SetParentIndex(GLuint index);
//    void Clear();
//    void GetTransformation(GLMatrix4f &transformation, const GLMatrix4f &parentTransformation = GLMatrix4f::Identity());
////private:
//    std::string mBoneName;
//    GLuint mParentIndex;
//    GLuint mCurrentTick;
//    GLMatrix4f mBaseTransformation;
//    std::vector<TranslationSample > mTranslationSamples;
//    std::vector<QuaternionSample > mQuaternionSamples;
//    std::vector<ScaleSample > mScaleSamples;
////    std::vector<Bone * > mpChildren;
//};

//class Skeleton
//{
//public:
//    Skeleton(){}
//    virtual ~Skeleton();
//    void Clear();
//    void Init(const aiScene *pScene);
//    void BuildAnimationListRecursive(const aiNode *pNode, GLuint &baseIndex);
//    std::vector<BoneAnimation > mBoneAnimations;
//    std::unordered_map<std::string, GLuint > mBoneMap;
//};


class Mesh
{
    //trivial mesh piece handler. 
    //So far, it take mesh data with positions, one channel of texure coordinate, and normal, with index array.
    //Data other than this format will be rejected.
    //assimp will load normal anyway. If the data does not have normal, assimp will generate it from geometry.
public:
    Mesh();
    Mesh(
            const std::vector<GLfloat>& positions,
            const std::vector<GLfloat>& normals,
            const std::vector<GLfloat>& textureCoordinates,
            const std::vector<GLubyte> &boneIds,
            const std::vector<GLfloat> &boneWeights,
            const std::vector<GLuint>& indices,
            GLuint materialIndex = 0,
            GLenum usage = GL_STATIC_DRAW
            );
    virtual ~Mesh();
    void Init(
            const std::vector<GLfloat>& positions,
            const std::vector<GLfloat>& normals,
            const std::vector<GLfloat>& textureCoordinates,
            const std::vector<GLubyte> &boneIds,
            const std::vector<GLfloat> &boneWeights,
            const std::vector<GLuint>& indices,
            GLuint materialIndex = 0,
            GLenum usage = GL_STATIC_DRAW
            );
//    void Init(const std::vector<GLfloat > &poistions,
//              const std::vector<GLfloat > &normals,
//              const std::vector<GLfloat > &textureCoordinates,
//              const std::vector<GLubyte > &boneIds,
//              const std::vector<GLfloat > &boneWeights,
//              const std::vector<GLuint > &indices,
//              GLuint materialIndex = 0,
//              GLenum usage = GL_STATIC_DRAW
//            );
    void Clear();
    void Render();//render the mesh
    void DrawOn();//core part for rendering for performance, without glEnableVertexAttribArray
    GLuint GetVbo(){return mVbo;}
    GLuint GetIbo(){return mIbo;}
    GLuint GetMaterialIndex(){return mMaterialIndex;}
private:
    GLuint mVbo;
    GLuint mIbo;
    GLuint mDataBuffers[NUM_DBs];
    GLuint mNumIndices;
    GLuint mNumVertices;
    GLuint mMaterialIndex;
};

class TranslationSample
{
public:
    GLfloat time;
    GLVector3f translation;
};

struct QuaternionSample
{
    GLfloat time;
    GLQuaternionf quaternion;
};

struct ScaleSample
{
    GLfloat time;
    GLVector3f scale;
};

struct NodeAnimation
{
public:
//    NodeAnimation();
//    virtual ~NodeAnimation();
    void InterpolateTransformation(GLfloat time);

    std::string nodeName;
    std::vector<TranslationSample > translationSamples;
    std::vector<QuaternionSample > quaternionSamples;
    std::vector<ScaleSample > scaleSamples;
    GLMatrix4f offsetTransformation;
    GLMatrix4f currentTransformation;
    GLMatrix4f finalTransformation;
    GLuint translationTick;
    GLuint quaternionTick;
    GLuint scaleTick;
};

struct UnitAction
{
    GLfloat duration;
    std::string name;
    std::vector<NodeAnimation > animationList;
    GLfloat timePerTick;
    GLfloat initTimeInSecond;
};

class UnitNode
{
public:
    UnitNode();
    virtual ~UnitNode();
//private:
    std::string mName;
    GLboolean mHasNodeAnimation;
    GLubyte mAnimationIndex;
    GLMatrix4f mTransformation;
    std::vector<UnitNode *> mChildren;
};

class UnitTree
{
public:
    UnitTree();
    virtual ~UnitTree();
    void BuildFromAssimpRecursive(UnitNode *&pNode, const aiNode *pAiNode);
    void InitAction(const aiScene *pScene);
    void Init(const aiScene *pScene);
    void UpdateAnimation(GLfloat time);
    void UpdateAnimationRecursive(UnitNode *pNode, const GLMatrix4f &parentTransformation, GLfloat time);
    void Clear();

    std::unordered_map<std::string, GLubyte> mAnimationMap;
    UnitNode *mRootNode;
    GLubyte mActionIndex;
    UnitAction mAction;
    GLMatrix4f mGlobalInverseTransform;

    //for testing
    void Print();
    void PrintRecursive(const UnitNode *pNode, int depth);
};

class RenderUnit
{
public:
    RenderUnit();
    virtual ~RenderUnit();
    void InitFromMemory(const std::string &fileData);
    void Clear();
    void DrawOn();//draw core part
    void Render();//full render function
    void SetShaderIndex(GLuint shaderIndex){mShaderIndex = shaderIndex;}
    void SetUpShader();
    void setResourcePath(std::string path);
private:
//    class BoneInfo
//    {
//        Eigen::Matrix<GLfloat, 4, 4, Eigen::ColMajor> mBoneOffset;
//        Eigen::Matrix<GLfloat, 4, 4, Eigen::ColMajor> mFinalTransformation;
//        const aiBone mpBone;
//        const aiNodeAnim mpNodeAnimation;

//        BoneInfo()
//        {
//            mBoneOffset.setIdentity();
//            mFinalTransformation.setIdentity();
//            //mpNodeAnimation = NULL;
//        }
//    };
    
    void InitMeshes(const aiScene* pScene);
    void InitTextures(const aiScene* pScene);
    void InitAnimations(const aiScene* pScene);
    void UpdateAnimationTick();
    std::vector<Mesh *> mMeshPtrList;
    std::vector<Texture *> mTexturePtrList;
    UnitTree mAnimationTree;
    std::chrono::time_point<std::chrono::high_resolution_clock> mInitTimeInSecond, mCurrentTimeInSecond;
    GLubyte mBoneLocation[MAX_BONES];
    GLuint mShaderIndex;
    std::string mResourcePath;
//    GLfloat mInitTimeInSecond;
//    std::vector<BoneInfo> mBoneInfoList;
//    std::unordered_map<std::string, GLubyte > mBoneMap;
//    const aiScene* mpScene;
//    Assimp::Importer mImporter;
};

}



#endif	/* MESH_H */

