///// PROPOSED NEW IMPLEMENTATION
//

#include "skeleton.h"

///---------------------------------------------------

Skeleton::Skeleton() : num_bones(0),
                       bones(nullptr),
                       num_anims(0),
                       animations(nullptr)
{
    //ctor
}

Skeleton::~Skeleton()
{
    delete [] bones;
    delete [] animations;
}

void Skeleton::fromFile(std::string skel_key)
{
    bool debug = false;

    std::string filepath = "assets_bin/skeletons/"+skel_key+".bbns";

    std::streampos filesize;
    char * memblock;

    std::ifstream myFile (filepath, std::ios::in | std::ios::binary | std::ios::ate);

    if (myFile.is_open())
    {
        filesize = myFile.tellg();
        memblock = new char [filesize];
        myFile.seekg (0, std::ios::beg);
        myFile.read (memblock, filesize);
        myFile.close();

        if (debug) std::cout << filesize << "\n";

        std::string filetype;
        filetype.resize(4*sizeof(char));      /// 4 bytes     file type
        int verMaj;                           /// 4 bytes     version major
        int verMin;                           /// 4 bytes     version minor

        MemStreamer reader(memblock, 0, filesize);
        reader.chomp(&filetype[0],  4*sizeof(char));
        reader.chomp(&verMaj,       1*sizeof(int));
        reader.chomp(&verMin,       1*sizeof(int));


        if (debug) std::cout << filetype << "\n";
        if (debug) std::cout << verMaj << "\n";
        if (debug) std::cout << verMin << "\n";

        /// READ NUMBER OF BONES
        reader.chomp(&num_bones,       1*sizeof(int));
        if (debug) std::cout << "num of bones: " << num_bones << "\n";

        // init bones
        bones = new Bone [num_bones];

        for (int u = 0; u<num_bones; u++)
        {
            int bone_index_u;
            reader.chomp(&bone_index_u,       1*sizeof(int));
            if (debug) std::cout << "bone index: "<< bone_index_u << "\n";

            Bone* bone = &bones[u];

            /// Write the name
            int nameLen;
            reader.chomp(&nameLen, 1*sizeof(int));

            bone->name.resize(nameLen*sizeof(char));      /// 4 bytes     file type
            reader.chomp(&bone->name[0],       nameLen*sizeof(char));
            if (debug) std::cout << "name: " << bone->name << "\n";

            /// READ THE BONE MATRIX
            float matarr[16];
            reader.chomp(&matarr[0],       16*sizeof(float));
            for (int i = 0; i<4; i++)
            {
                for (int j = 0; j<4; j++)
                {
                    bone->rest_matrix[i][j] = matarr[j+4*i]; // Store row major
                    if (debug) std::cout << bone->rest_matrix[i][j] << "\t";
                }
                if (debug) std::cout << "\n";
            }

            reader.chomp(&bone->num_children, 1*sizeof(int));
            if (debug) std::cout << "num children: " << bone->num_children<<"\n";

            bone->child_indices = new int [bone->num_children];
            reader.chomp(bone->child_indices, bone->num_children*sizeof(int));
            if (debug) std::cout << "children: ";
            for (int i = 0; i<bone->num_children; i++) if (debug) std::cout << bone->child_indices[i] << "\t";
            if (debug) std::cout << "\n";
        } /// Finished loading bones

        /// Animations
        reader.chomp(&num_anims, 1*sizeof(int));
        if (debug) std::cout << "num anims: " << num_anims << "\n";

        animations = new Animation [num_anims];

        for (int u = 0; u<num_anims; u++)
        {
            int anim_index_u;
            reader.chomp(&anim_index_u,       1*sizeof(int));
            Animation* anim = &animations[anim_index_u];
            if (debug) std::cout << "anim index: " << anim_index_u << "\n";

            /// Duration
            reader.chomp(&anim->duration, 1*sizeof(float));
            if (debug) std::cout<<"duration: " << anim->duration << "\n";

            /// Number of channels
            reader.chomp(&anim->num_channels, 1*sizeof(int));
            anim->channels = new Animation::Channel [anim->num_channels];
            if (debug) std::cout << "num channels: " << anim->num_channels << "\n";

            for (int c = 0; c<anim->num_channels; c++)
            {
                int channel_index_c;
                reader.chomp(&channel_index_c,       1*sizeof(int));
                Animation::Channel* channel = &anim->channels[channel_index_c];
                if (debug) std::cout << "channel index: " << channel_index_c << "\n";
                channel->bone_index = channel_index_c;

                /// Read number of position keys
                reader.chomp(&channel->pos_series.num_keys,       1*sizeof(int));
                channel->pos_series.keys = new TimeValuePair<glm::vec3> [channel->pos_series.num_keys];
                channel->pos_series.duration = anim->duration;
                if (debug) std::cout << "num pos keys: " << channel->pos_series.num_keys << "\n";

                for (int k = 0; k<channel->pos_series.num_keys; k++)
                {
                    /// Time
                    TimeValuePair<glm::vec3>* timePosPair = &channel->pos_series.keys[k];
                    reader.chomp(&timePosPair->time, 1*sizeof(float));
                    if (debug) std::cout << "time: " << timePosPair->time << "\n";

                    /// Vector
                    reader.chomp(&(timePosPair->value[0]), 3*sizeof(float));
                    if (debug) std::cout << "pos: [" << timePosPair->value.x << "\t"
                                          << timePosPair->value.y << "\t"
                                          << timePosPair->value.z << "]\n";
                }

                /// Read number of rotation keys
                reader.chomp(&channel->rot_series.num_keys,       1*sizeof(int));
                channel->rot_series.keys = new TimeValuePair<glm::quat> [channel->rot_series.num_keys];
                channel->rot_series.duration = anim->duration;
                if (debug) std::cout << "num rot keys: " << channel->rot_series.num_keys << "\n";

                for (int k = 0; k<channel->rot_series.num_keys; k++)
                {
                    /// Time
                    TimeValuePair<glm::quat>* timeRotPair = &channel->rot_series.keys[k];
                    reader.chomp(&timeRotPair->time, 1*sizeof(float));
                    if (debug) std::cout << "time: " << timeRotPair->time << "\n";

                    /// Quaternion
                    reader.chomp(&(timeRotPair->value.w), 1*sizeof(float));
                    reader.chomp(&(timeRotPair->value.x), 1*sizeof(float));
                    reader.chomp(&(timeRotPair->value.y), 1*sizeof(float));
                    reader.chomp(&(timeRotPair->value.z), 1*sizeof(float));
                    if (debug) std::cout << "rot: [" << timeRotPair->value.w << "\t"
                                          << timeRotPair->value.x << "\t"
                                          << timeRotPair->value.y << "\t"
                                          << timeRotPair->value.z << "]\n";
                }

                /// Read number of scaling keys
                reader.chomp(&channel->sca_series.num_keys,       1*sizeof(int));
                channel->sca_series.keys = new TimeValuePair<glm::vec3> [channel->sca_series.num_keys];
                channel->sca_series.duration = anim->duration;
                if (debug) std::cout << "num sca keys: " << channel->sca_series.num_keys << "\n";

                for (int k = 0; k<channel->sca_series.num_keys; k++)
                {
                    /// Time
                    TimeValuePair<glm::vec3>* timeScaPair = &channel->sca_series.keys[k];
                    reader.chomp(&timeScaPair->time, 1*sizeof(float));
                    if (debug) std::cout << "time: " << timeScaPair->time << "\n";

                    /// Vector
                    reader.chomp(&(timeScaPair->value[0]), 3*sizeof(float));
                    if (debug) std::cout << "sca: [" << timeScaPair->value.x << "\t"
                                          << timeScaPair->value.y << "\t"
                                          << timeScaPair->value.z << "]\n";
                }
                /// Fill helper variable (used for seek)
                channel->ch_duration = anim->duration;
            }
        }

        delete[] memblock;
//        delete filetype;
    }
    else
    {
        std::cout << "failed to open file " << filepath << "\n";
    }
}

void Skeleton::poseMatrices(glm::mat4* matrices,
                            int anim_index,
                            float time,
                            int bone_index,
                            glm::mat4 parent_mat)
{
    bool debug = true;

    if (anim_index<num_anims && anim_index > -1 && num_bones > 0)
    {
        Animation* animation = &animations[anim_index];

        Bone* bone = &bones[bone_index];
        Animation::Channel* channel = &(animation->channels[bone_index]);

        /// No interpolation, just use previous frame
//        int prev_pos_key = channel->seekPrevPos(time);
//        int prev_rot_key = channel->seekPrevRot(time);
//        int prev_sca_key = channel->seekPrevSca(time);
//
//        int next_rot_key

//        glm::vec3 key_pos = channel->pos_keys[channel->seekPrevPos(time)].pos;
//        glm::quat key_rot = channel->rot_keys[channel->seekPrevRot(time)].rot;
//        glm::vec3 key_sca = channel->sca_keys[channel->seekPrevSca(time)].sca;

//        glm::vec3 key_pos = channel->pos_series.seekPrev(time)->value;
//        glm::quat key_rot = channel->rot_series.seekPrev(time)->value;
//        glm::vec3 key_sca = channel->sca_series.seekPrev(time)->value;

        auto pos_res = channel->pos_series.seekPrev(time);
        auto rot_res = channel->rot_series.seekPrev(time);
        auto sca_res = channel->sca_series.seekPrev(time);

//        glm::vec3 key_pos = pos_res.prev.key->value;
//        glm::quat key_rot = rot_res.prev.key->value;
//        glm::vec3 key_sca = sca_res.prev.key->value;

        glm::vec3 key_pos = glm::mix(pos_res.prev.key->value, pos_res.next.key->value, pos_res.next.weight);
        glm::quat key_rot = glm::mix(rot_res.prev.key->value, rot_res.next.key->value, rot_res.next.weight);
        glm::vec3 key_sca = glm::mix(sca_res.prev.key->value, sca_res.next.key->value, sca_res.next.weight);

////
//        if (debug) std::cout << "INTERPOLATED: " << bone_index << " 0key:\n" ;
//        if (debug) std::cout << "pos: [" << key_pos.x << "\t"
//                              << key_pos.y << "\t"
//                              << key_pos.z << "]\n";
//
//        if (debug) std::cout << "rot: [" << key_rot.w << "\t"
//                              << key_rot.x << "\t"
//                              << key_rot.y << "\t"
//                              << key_rot.z << "]\n";
//
//        if (debug) std::cout << "sca: [" << key_sca.x << "\t"
//                              << key_sca.y << "\t"
//                              << key_sca.z << "]\n";

        glm::mat4 trans_mat = glm::translate(glm::mat4(1.0),
                                             key_pos);
        glm::mat4 rot_mat = glm::mat4_cast(key_rot);
        glm::mat4 sca_mat = glm::scale(glm::mat4(1.0),
                                       key_sca);

        glm::mat4 local_mat = trans_mat * rot_mat * sca_mat;

        glm::mat4 pose_mat = parent_mat * local_mat;


        /// Why the f*** do these need to be transposed?
        /// isnt glm supposed to reflect glsl?
        matrices[bone_index] = glm::transpose(pose_mat * glm::inverse(bones[bone_index].rest_matrix));
//        matrices[bone_index] = pose_mat;
//        matrices[bone_index] = bones[bone_index].rest_matrix;

//        if (debug) std::cout << "bone index: " << bone_index << " local_mat:\n" ;
//        for (int i = 0; i<4; i++)
//        {
//            for (int j = 0; j<4; j++)
//            {
//                if (debug) std::cout << local_mat[j][i] << "\t";
//            }
//            if (debug) std::cout << "\n";
//        }
//        if (debug) std::cout << "bone index: " << bone_index << " rest_mat:\n" ;
//        for (int i = 0; i<4; i++)
//        {
//            for (int j = 0; j<4; j++)
//            {
//                if (debug) std::cout << bones[bone_index].rest_matrix[j][i] << "\t";
//            }
//            if (debug) std::cout << "\n";
//        }

        for (int i_child=0; i_child<bone->num_children; i_child++)
        {
            poseMatrices(matrices,
                         anim_index,
                         time,
                         bone->child_indices[i_child],
                         pose_mat);
        }

    }
    else
    {
        std::cerr << "invalid skeleton pose call\n";
        std::cerr << "setting all bone matrices to rest\n";
        for (int i=0; i<num_bones; i++)
        {
            matrices[i] = bones[i].rest_matrix;
        }
    }

}


///---------------------------------------------------



Skeleton::Animation::Animation() : num_channels(0),
                                   channels(nullptr)
{
    //ctor
}

Skeleton::Animation::~Animation()
{
    delete [] channels;
}


///---------------------------------------------------


Skeleton::Animation::Channel::Channel()
{
    //ctor
}

Skeleton::Animation::Channel::~Channel()
{

}

///---------------------------------------------------


Skeleton::Pose::Pose() : num_transforms(0),
                         transforms(nullptr)
{
    // ctor
}

Skeleton::Pose::~Pose()
{
    delete [] transforms;
}

Skeleton::Pose::Pose(int num_transforms_in)
{
    allocate(num_transforms_in);
}

Skeleton::Pose::Pose(Skeleton* skel)
{
    allocate(skel->getNumBones());
}

void Skeleton::Pose::allocate(int num_transforms_in)
{
    num_transforms = num_transforms_in;
    transforms = new Transform [num_transforms];
}













//
//#include "skeleton.h"
//
//Animation::Animation()
//{
//    number = -1;//ctor
//}
//
//const string Animation::anim_name[] =
//{
//    "stand",
//    "walk",
//    "run",
//    "crouch"
//};
//
//Animation::Animation(string name_in, float start, float end)
//{
//    name = name_in;
//    start_Frame = start;
//    end_Frame = end;
//    int indexcandidate = -1;
//    for (int i = 0; i<ANIMS_NUM; i++)
//    {
//        if (name == anim_name[i])
//        {
//            indexcandidate = i;
//        }
//    }
//    if (indexcandidate==-1)
//    {
//        cerr<<"animation not supported: "<<name_in<<"\n";
//    }
//    number = indexcandidate;
////    cout<<"animation "<<name_in<<" ("<<start<<": "<<end<<") \n"; // uncommenting this gives a massive increase in weird vertices
//}
//
//Animation::~Animation()
//{
//    //dtor
//}
//
//KeyFrame::KeyFrame(float frame_in, float value_in)
//{
//    frame = frame_in;
//    value = value_in;
//}
//
//KeyFrame::KeyFrame()
//{
//    frame = -1; // means empty
//}
//
//
//Bone::Bone()
//{
//    //cind
//}
//
//Bone::~Bone()
//{
//
//};
//
//Skeleton::Skeleton()
//{
//    animations.resize(Animation::ANIMS_NUM);
//    currentFrame = 0.0;
//    fired = false;
//    //ctor
//}
//
//const float Skeleton::DEBUG_FRAME = 1.0;
//
//Skeleton::~Skeleton()
//{
//    //delete [] bones;
//}
//
//void Skeleton::addAnim(Animation anim_in)
//{
//    int num = anim_in.number;
//    animations[num] = (anim_in);
//}
//
//void Skeleton::advance_Interpolation(float dt)   // must start an animation
//{
//    currentFrame += dt;
//    float t;
//    int offs = 1;
////    if (currentFrame > animations[ActiveAnimationIndex].end_Frame - 1 &&  currentFrame < animations[ActiveAnimationIndex].end_Frame) {
////        rightKeyIndex = animations[ActiveAnimationIndex].start_Frame;
////        cout<<"hit\n";
////    }
//    if (currentFrame > animations[ActiveAnimationIndex].end_Frame && currentFrame < animations[ActiveAnimationIndex].end_Frame+offs)
//    {
////        startAnimation(animations[ActiveAnimationIndex].number); // should not start animation
////        currentFrame = currentFrame - (animations[ActiveAnimationIndex].end_Frame-animations[ActiveAnimationIndex].start_Frame);
//        leftKeyIndex = animations[ActiveAnimationIndex].end_Frame;
//        rightKeyIndex = animations[ActiveAnimationIndex].start_Frame;
//        t = (currentFrame-(float)leftKeyIndex)/((float)offs);
//    }
//    else if (currentFrame > animations[ActiveAnimationIndex].end_Frame+offs)
//    {
//        currentFrame = currentFrame - (animations[ActiveAnimationIndex].end_Frame-animations[ActiveAnimationIndex].start_Frame + offs);
//        leftKeyIndex = animations[ActiveAnimationIndex].start_Frame;
//        findRightInd();
//        t = (currentFrame-(float)leftKeyIndex)/((float)rightKeyIndex-(float)leftKeyIndex);
//    }
//    else
//    {
//        if (ceil(currentFrame>rightKeyIndex))
//        {
//            // find the new interpolation limits
//            leftKeyIndex = rightKeyIndex;
//            findRightInd();
//        }
//        t = (currentFrame-(float)leftKeyIndex)/((float)rightKeyIndex-(float)leftKeyIndex);
//    }
//    //cout<<"t="<<currentFrame-dt<<"; dt="<<dt<<" left:"<<leftKeyIndex<<" right:"<<rightKeyIndex<<"\n";
//
//
//
//
//
//    for (int bn = 0; bn<bones_rest.size(); bn++)
//    {
//        // interpolates
////        if (bn ==29) {
////            cout<<bones_rest[bn].name;
////        }
//
////        cout<<"bn = "<<bones_rest[bn].name<<", bnmax = "<<bones_rest.size()<<", leftKeyInd = "<<leftKeyIndex<<"keyPosmax = "<<bones_rest[bn].keyPositions[0].size()<<"\n";
//        float px1 = bones_rest[bn].keyPositions[0][leftKeyIndex].value;
//        float py1 = bones_rest[bn].keyPositions[1][leftKeyIndex].value;
//        float pz1 = bones_rest[bn].keyPositions[2][leftKeyIndex].value;
//
//        float px2 = bones_rest[bn].keyPositions[0][rightKeyIndex].value;
//        float py2 = bones_rest[bn].keyPositions[1][rightKeyIndex].value;
//        float pz2 = bones_rest[bn].keyPositions[2][rightKeyIndex].value;
//
//        float pxi = px1 + t*(px2 - px1);
//        float pyi = py1 + t*(py2 - py1);
//        float pzi = pz1 + t*(pz2 - pz1);
//        vec3 p = vec3(pxi, pyi, pzi);
//
//        float w1 = bones_rest[bn].keyPositions[3][leftKeyIndex].value;
//        float x1 = bones_rest[bn].keyPositions[4][leftKeyIndex].value;
//        float y1 = bones_rest[bn].keyPositions[5][leftKeyIndex].value;
//        float z1 = bones_rest[bn].keyPositions[6][leftKeyIndex].value;
//        Quat q1 = Quat(w1, x1, y1, z1);
//
//        float w2 = bones_rest[bn].keyPositions[3][rightKeyIndex].value;
//        float x2 = bones_rest[bn].keyPositions[4][rightKeyIndex].value;
//        float y2 = bones_rest[bn].keyPositions[5][rightKeyIndex].value;
//        float z2 = bones_rest[bn].keyPositions[6][rightKeyIndex].value;
//        Quat q2 = Quat(w2, x2, y2, z2);
//
//        Quat q = Quat::slerp(q1,q2,t);
//        slerped_rots[bn]=q;
//        lerped_posits[bn]=p;
//
//        // THIS COULD BE TESTED: WITH ONLY ACTORS HERE, 100 AND STILL 60 FPS, 500 gives some serious overhead.
//        // THERE IS NO INSTANCING OF SKELETON DURING LOADING. --> BAD
//        // NOW ALL THAT IS REMAINING IS TO PASS SLERPS AND LERPS TO VERTEX SHADER AND THEN WRITE A VERTEX SHADER THAT APPLIES THE TRANSFORMATIONS!
//        // Multiply all the bone matrices together.
//
//        // now to the bone matrices;
////        mat4 Th = (Transform::translate(bones_rest[root_index].head));
////        mat4 Tmh =  (Transform::translate(-bones_rest[root_index].head));
//        mat4 Tk =  (Transform::translate(lerped_posits[bn])); // ANIMATION SYSTEM IS NOW APPARENTLY FINISHED
////        Tk = mat4(1.0); // Line means only ROTATION for bones.
//        mat4 Rk =  (Transform::quatRot(slerped_rots[bn]));
//
//        bone_delta_mats[bn] = mm::matMult(Tk, Rk);
////        bone_delta_mats[bn] = mat4(1.0);
////        if (!fired && (abs(currentFrame-DEBUG_FRAME))<0.1 && bn==18) {
////            cout<<"Bone delta mat for bone # "<<bn<<": "<<bones_rest[bn].name<<"\n";
////            mm::matPrint(bone_delta_mats[bn]);
////
////        }
//    }
//    //mm::matPrint(bone_rest_mat);
//
//    mat4 bone_rest_mat = bones_rest[root_index].rest_matrix;
//    mat4 bone_rest_inv = glm::inverse(bone_rest_mat);
//
//    //bone_matrices[root_index] = Tk * Tmh * Rk * Th;
//    bone_matrices[root_index] = mm::matMult(bone_rest_mat, mm::matMult(bone_delta_mats[root_index], bone_rest_inv));
//    Ds[root_index] = mm::matMult(bone_rest_mat, bone_delta_mats[root_index]);
////    if (!fired && (abs(currentFrame-DEBUG_FRAME))<0.1) {
////        cout<<"Bone "<<root_index<<": "<<bones_rest[root_index].name<<" pose matrix \n";
////        mm::matPrint(bone_matrices[root_index]);
////    }
//    //bone_matrices[root_index] = mat4(1.0);
//
//    rec_bone_matrices(root_index);
//    for (int yy = 0; yy<numBones; yy++)
//    {
//        norm_matrices[yy] = glm::inverse(glm::transpose(bone_matrices[yy]));
//    }
//
//    if (!fired && (abs(currentFrame-DEBUG_FRAME))<0.1)
//    {
//        fired = true;
//    }
//
//};
//
//
///** TEST USING TEST ACTOR **/
//
//void Skeleton::rec_bone_matrices(int ind)   // THIS IS THE NEXT GREAT TEST
//{
////    cout<<"recursive call for "<<bones_rest[ind].name<<"\n";
////    cout<<"numChildren equals "<<bones_rest[ind].numChildren<<"\n\n";
//    for (int i = 0; i< bones_rest[ind].numChildren; i++)
//    {
//        int cind = bones_rest[ind].childIndex[i];
//
//        mat4 delta_i = bone_delta_mats[cind];
////        mat4 delta_i = mat4(1.0);
//
//        mat4 Bi_rest_inv = glm::inverse(bones_rest[cind].rest_matrix); // GLM inverse wrong? NO
//        mat4 delta_rest_i = bones_rest[cind].rest_del_mat;
//
//        mat4 Dim1 = Ds[ind];
//        mat4 Di = mm::matMult(Dim1, mm::matMult(delta_rest_i, delta_i));
//        Ds[cind] = Di;
//        bone_matrices[cind] = mm::matMult(Di, Bi_rest_inv);
//
//        if (!fired && (abs(currentFrame-DEBUG_FRAME))<0.1)
//        {
////            cout<<"Bone "<<cind<<": "<<bones_rest[cind].name<<" pose matrix \n";
////            mm::matPrint(bone_matrices[cind]);
////            cout<<"Bone "<<cind<<": "<<bones_rest[cind].name<<" delta_i \n";
////            mm::matPrint(delta_i);
////            cout<<"Bone "<<ind<<": "<<bones_rest[cind].name<<" parent rest_del_mat \n";
////            mm::matPrint(bones_rest[ind].rest_matrix);
////            cout<<"Bone "<<ind<<": "<<bones_rest[cind].name<<" parent bone_mat \n";
////            mm::matPrint(Dim1);
////            cout<<"Bone "<<cind<<": "<<bones_rest[cind].name<<" child bone_rest \n";
////            mm::matPrint(bones_rest[cind].rest_matrix);
////            cout<<"Bone "<<cind<<": "<<bones_rest[cind].name<<" child delta_rest_i \n";
////            mm::matPrint(delta_rest_i);
////            cout<<"Bone "<<cind<<": "<<bones_rest[cind].name<<" child delta_i \n";
////            mm::matPrint(delta_i);
////            cout<<"Bone "<<cind<<": "<<bones_rest[cind].name<<" child bone_mat \n";
////            mm::matPrint(Di);
////            cout<<"Bone "<<cind<<": "<<bones_rest[cind].name<<" final_matrix \n";
////            mm::matPrint(bone_matrices[cind]);
//        }
//
//        rec_bone_matrices(cind);
//    }
//}
//
//void Skeleton::rec_bone_matrices_rest(int ind, vector<mat4> &temp_matrices)
//{
////    cout<<"recursive call for "<<bones_rest[ind].name<<"\n";
////    cout<<"numChildren equals "<<bones_rest[ind].numChildren<<"\n\n";
////    cout<<"bone :"<<bones_rest[ind].name<<" has "<<bones_rest[ind].numChildren<<"children \n";
//    for (int i = 0; i< bones_rest[ind].numChildren; i++)
//    {
//        int cind = bones_rest[ind].childIndex[i];
//
//        mat4 parent_rest_matrix = bones_rest[ind].rest_matrix;
////        mat4 inv_parent_rest_matrix = glm::inverse(parent_rest_matrix);
//
//        vec3 head2head = bones_rest[cind].head-bones_rest[ind].head;
//        vec4 head2head4 = vec4(head2head.x, head2head.y, head2head.z, 0.0);
//        vec4 head2head4_parent_space = parent_rest_matrix * head2head4;
//        vec3 head2head3_parent_space = vec3(head2head4_parent_space.x, head2head4_parent_space.y, head2head4_parent_space.z);
//
//        mat4 T = Transform::translate(head2head);
//
//        bones_rest[cind].rest_matrix = mm::matMult(T, mm::matMult(bones_rest[ind].rest_matrix, temp_matrices[cind]));
//
//        bones_rest[cind].rest_del_mat = mm::matMult(Transform::translate(head2head3_parent_space), temp_matrices[cind]); // this is wrong!!!
//
//
//        /** THESE ARE NOT THE SAME ONCE ROLL IS INCLUDED - test **/
////        if (cind < 12) { //
////            cout<<"rest matrix for bone: "<<bones_rest[cind].name<<": \n";
//////            mm::matPrint(B)
////            mm::matPrint(bones_rest[cind].rest_matrix); // seems right
////
////            mat4 accDelMats = mat4(1.0);
////            cout<<"multiplying together: \n";
////            mm::matPrint(accDelMats);
////            for (int cc = 0; cc<cind+1; cc++) {
////                cout<<"* \n";
////                mm::matPrint(bones_rest[cc].rest_del_mat);
////                accDelMats = mm::matMult(accDelMats, bones_rest[cc].rest_del_mat);
////            }
////            cout<<"= \n";
////            mm::matPrint(accDelMats); // seems right
////
////        }
//        rec_bone_matrices_rest(cind, temp_matrices);
//
//    }
//}
//
//void Skeleton::rec_bone_matrices_rest_rolls(int ind, vector<float> &rolls)
//{
////    cout<<"recursive call for "<<bones_rest[ind].name<<"\n";
////    cout<<"numChildren equals "<<bones_rest[ind].numChildren<<"\n\n";
////    cout<<"bone :"<<bones_rest[ind].name<<" has "<<bones_rest[ind].numChildren<<"children \n";
//    for (int i = 0; i< bones_rest[ind].numChildren; i++)
//    {
//        int cind = bones_rest[ind].childIndex[i];
//
//        mat4 parent_rest_matrix = bones_rest[ind].rest_matrix;
////        mat4 inv_parent_rest_matrix = glm::inverse(parent_rest_matrix);
//
//        vec3 head2head = bones_rest[cind].head-bones_rest[ind].head;
//        vec4 head2head4 = vec4(head2head.x, head2head.y, head2head.z, 0.0);
//        vec4 head2head4_parent_space = parent_rest_matrix * head2head4;
//        vec3 head2head3_parent_space = vec3(head2head4_parent_space.x, head2head4_parent_space.y, head2head4_parent_space.z);
//
//        vec3 head2tail3 = bones_rest[cind].tail-bones_rest[cind].head;
//        vec4 head2tail4 = vec4(head2tail3.x, head2tail3.y, head2tail3.z, 0.0);
//        vec4 head2tail4_parent_space = parent_rest_matrix * head2tail4;
//        vec3 head2tail3_parent_space = vec3(head2tail4_parent_space.x, head2tail4_parent_space.y, head2tail4_parent_space.z);
//
//
//        mat4 T = Transform::translate(head2head);
//        mat4 T_parent_space = Transform::translate(head2head3_parent_space);
//
//        mat3 roll_rot = Transform::rotate(rolls[cind], vec3(0, 1, 0));
//        mat4 roll_rot4 = mm::mat3_to_mat4(roll_rot);
//
//        vec3 head_tail_rot_axis3 = glm::normalize(glm::cross(vec3(0, 1, 0), head2tail3_parent_space)); // This cross product might go the wrong way
////        vec4 head_tail_rot_axis3 = vec3(head_tail_rot_axis.x, head_tail_rot_axis.y, head_tail_rot_axis.z);
//        float head_tail_rot_angle = 180/3.14159265*acos(head2tail4_parent_space.y/(glm::length(head2tail4_parent_space))); // this in degrees or radians?
//
//        mat4 head_tail_rot = mm::mat3_to_mat4(Transform::rotate(head_tail_rot_angle, head_tail_rot_axis3));
//
//        mat4 rotation_parent_space = mm::matMult(head_tail_rot, roll_rot4);
//
//        bones_rest[cind].rest_matrix = mm::matMult(T, mm::matMult(bones_rest[ind].rest_matrix, rotation_parent_space));
//
//        bones_rest[cind].rest_del_mat = mm::matMult(T_parent_space, rotation_parent_space); //
//
//
//        /** THESE ARE NOT THE SAME ONCE ROLL IS INCLUDED - test - STILL NOT THE SAME **/
////        if (cind < 12) { //
////            cout<<"bone: "<<bones_rest[cind].name<<": \n";
////////            mm::matPrint(B)
////            mm::matPrint(bones_rest[cind].rest_matrix); // seems right
////
////            mat4 accDelMats = mat4(1.0);
////            cout<<"multiplying together: \n";
////            mm::matPrint(accDelMats);
////            for (int cc = 0; cc<cind+1; cc++) {
////                cout<<"* \n";
////                mm::matPrint(bones_rest[cc].rest_del_mat);
////                accDelMats = mm::matMult(accDelMats, bones_rest[cc].rest_del_mat);
////            }
////            cout<<"= \n";
////            mm::matPrint(accDelMats); // seems right
//////              cout<<"head2head \t";
//////              mm::vecPrint(head2head);
//////              cout<<"head2head_p_s \t";
//////              mm::vecPrint(head2head3_parent_space);
//////              cout<<"head2tail \t";
//////              mm::vecPrint(head2tail3);
//////              cout<<"head2tail_p_s \t";
//////              mm::vecPrint(head2tail3_parent_space);
////
////
////        }
//        rec_bone_matrices_rest_rolls(cind, rolls);
//
//    }
//}
//
//void Skeleton::findRightInd()
//{
//    for (int i = leftKeyIndex+1; i<bones_rest[0].keyPositions[0].size(); i++)
//    {
//        if (bones_rest[0].keyPositions[0][i].frame != -1)
//        {
//            rightKeyIndex = i;
//            break;
//        }
//    }
//}
//void Skeleton::jump2frame(float frame)
//{
//
//}; // Have an idea: Just convert keyframes to ints, make a vector to store frames as well as empty ones. Then use direct lookup...
//void Skeleton::startAnimation(int num)
//{
//    Animation anim = animations[num];
//    leftKeyIndex = anim.start_Frame;
//
//    ActiveAnimationIndex = num;
//
//    findRightInd();
//
//    currentFrame = anim.start_Frame;
//
//    //cout<<"currentFrame = "<< currentFrame<<", leftKeyIndex = "<<leftKeyIndex<<", rightKeyIndex = "<<rightKeyIndex<<"\n";
//};
//
//void Skeleton::loadFromBNS(std::string file_path)
//{
//    string str, ret = "", cmd;
//    int end = 0;
//
//    vector<string> parent_names;
//    vector<string> children_names;
//    vector<string> bone_anim_names;
//    vector<string> keyFrameStrings;
//    vector<mat4> bone_rot_rest;
//    vector<float> rolls;
//
//    ifstream in ;
//    in.open(file_path.c_str()) ;
//    if (in.is_open())
//    {
//        getline (in, str) ;
//        while (in)
//        {
//            if (!((str.substr(0,1)=="#") || (str=="")))
//            {
////                cout << str << "\n";
//                cmd = strtok(&str[0], " ");
//
//                if (cmd=="bone")
//                {
//                    Bone bone;
//                    bone.name = strtok(NULL, " ");
//                    bones_rest.push_back(bone);
//                    end = bones_rest.size()-1;
//                    //bones_rest[end].index =
//                }
//                if (cmd=="parent")
//                {
//                    string parent = strtok(NULL, " ");
//                    parent_names.push_back(parent);
//                }
//                if (cmd=="children")
//                {
//                    char* child = strtok(NULL, " ");
//                    string children_name = "";
//                    while (child != NULL)
//                    {
//                        string child_str = child;
//                        children_name +=child_str+" ";
//                        child = strtok (NULL, " ");
//                    }
//                    children_names.push_back(children_name);
//                }
//                if (cmd=="head")
//                {
//                    float x = atof(strtok(NULL, " "));
//                    float y = atof(strtok(NULL, " "));
//                    float z = atof(strtok(NULL, " "));
//                    bones_rest[end].head = vec3(x, y, z);
//                }
//                if (cmd=="tail")
//                {
//                    float x = atof(strtok(NULL, " "));
//                    float y = atof(strtok(NULL, " "));
//                    float z = atof(strtok(NULL, " "));
//                    bones_rest[end].tail = vec3(x, y, z);
//                }
//                if (cmd=="rotation_rest")
//                {
//                    // here the z and y rotation are switched
//                    mat4 rotmat;
//                    rotmat[0][0] = atof(strtok(NULL, " "));
//                    rotmat[0][1] = atof(strtok(NULL, " "));
//                    rotmat[0][2] = atof(strtok(NULL, " "));
//                    rotmat[0][3] = 0;
//
//                    string nl;
//
//                    getline(in, nl);
//                    rotmat[1][0] = atof(strtok(&nl[0], " "));
//                    rotmat[1][1] = atof(strtok(NULL, " "));
//                    rotmat[1][2] = atof(strtok(NULL, " "));
//                    rotmat[1][3] = 0;
//
//
//                    getline(in, nl);
//                    rotmat[2][0] = atof(strtok(&nl[0], " "));
//                    rotmat[2][1] = atof(strtok(NULL, " "));
//                    rotmat[2][2] = atof(strtok(NULL, " "));
//                    rotmat[2][3] = 0;
//
//                    rotmat[3][0] = 0;
//                    rotmat[3][1] = 0;
//                    rotmat[3][2] = 0;
//                    rotmat[3][3] = 1;
//
////                    				    mat4 rotmat;
////				    rotmat[0][0] = atof(strtok(NULL, " ")); rotmat[0][1] = atof(strtok(NULL, " ")); rotmat[0][2] = atof(strtok(NULL, " ")); rotmat[0][3] = 0;
////                    string nl;
////                    getline(in, nl);
////                    rotmat[1][0] = atof(strtok(&nl[0], " ")); rotmat[1][1] = atof(strtok(NULL, " ")); rotmat[1][2] = atof(strtok(NULL, " ")); rotmat[1][3] = 0;
////                    getline(in, nl);
////                    rotmat[2][0] = atof(strtok(&nl[0], " ")); rotmat[2][1] = atof(strtok(NULL, " ")); rotmat[2][2] = atof(strtok(NULL, " ")); rotmat[2][3] = 0;
////                    rotmat[3][0] = 0;                         rotmat[3][1] = 0;                       rotmat[3][2] = 0;                       rotmat[3][3] = 1;
//
//
////                    cout<<rotmat[0][0]<<", "<<rotmat[0][1]<<", "<<rotmat[0][2]<<"\n";
////                    cout<<rotmat[1][0]<<", "<<rotmat[1][1]<<", "<<rotmat[1][2]<<"\n";
////                    cout<<rotmat[2][0]<<", "<<rotmat[2][1]<<", "<<rotmat[2][2]<<"\n";
//                    bone_rot_rest.push_back(rotmat);
//                }
//                if (cmd=="roll")
//                {
//                    rolls.push_back(atof(strtok(NULL, " ")));
////                    cout<<"roll ";
////                    cout<<rolls[rolls.size()-1]<<"\n";
//                }
//                if (cmd=="frame_range")
//                {
//                    min_frame = atof(strtok(NULL, " "));
//                    max_frame = atof(strtok(NULL, " "));
//                }
//                if (cmd=="bone_anim")
//                {
//                    bone_anim_names.push_back(strtok(NULL, " "));
//                    for (int k = 0; k<10; k++)
//                    {
//                        string keyFrameStuff;
//                        getline(in, keyFrameStuff);
//                        keyFrameStrings.push_back(keyFrameStuff);
////                        cout<<keyFrameStuff<<"\n";
//                    }
//                }
//            }
//            getline (in, str) ;
//        }
//
//        numBones = end+1;
//
//        for (int i = 0; i<numBones; i++)
//        {
//            // Find the parent
//            int index_candidate = -1; // means no bone
//            string parent_name = parent_names[i];
//
//            for (int j = 0; j<numBones; j++)
//            {
//                if (bones_rest[j].name==parent_name)
//                {
//                    index_candidate = j;
//                }
//            }
//
//            bones_rest[i].parentIndex = index_candidate;
//            if (index_candidate==-1)
//            {
//                root_index = i;
//            }
//
//            // Find the children
//            int c_ind = 0;
//            char* child = strtok(&children_names[i][0], " ");
//            while (child != NULL)
//            {
//                string child_str = child;
//
//                int index_candidate_c = -1;
//                for (int k = 0; k<numBones; k++)
//                {
//                    if (bones_rest[k].name==child)
//                    {
//                        index_candidate_c = k;
//                    }
//                }
//
//                bones_rest[i].childIndex[c_ind] = index_candidate_c;
//                if (string(child) != "None")
//                {
//                    c_ind ++;
//                }
//
//                child = strtok (NULL, " ");
//            }
//            bones_rest[i].numChildren = c_ind;
//
//            /** FOR DEBUGGING PARENT-CHILD RELATIONS**//*cout<<"bone "<<i<<": "<<bones_rest[i].name<<" has parent: (bone "<<bones_rest[i].parentIndex<<": "<<bones_rest[bones_rest[i].parentIndex].name<<")\n";
//            cout<<"bone "<<i<<": "<<bones_rest[i].name<<" has children:";
//            for (int u = 0; u<bones_rest[i].numChildren; u++) {
//                cout<<"(bone "<<bones_rest[i].childIndex[u]<<": "<<bones_rest[bones_rest[i].childIndex[u]].name<<")\n";
//            } */
////            int debug_ind = 29;
//
//            // Doing the keyFrames;
//            index_candidate = -1;
//            for (int z = 0; z<numBones; z++)
//            {
////                if (i==debug_ind) {
////                    cout<<z<<":("<<bone_anim_names[z]<<", "<<bones_rest[i].name<<")\n";
////                }
////                cout<<"size of bone_anim_names is "<<bone_anim_names.size()<<"\n";
////                cout<<"size of bones_rest is "<<bones_rest.size()<<"\n";
////                cout<<"numBones is "<<numBones<<"\n";
//                if (bone_anim_names[z]==bones_rest[i].name)
//                {
//                    index_candidate = z;
//                    break;
//                }
//            }
//            int bones_anim_index = index_candidate;
//
//// problem here is that they are not in the same order as for the other bones
////            COMMENTED OUT FOR DEBUGGING ANIMATIONS
//
//            for (int u = 0; u<10; u++)
//            {
//                string cmd2 = strtok(&keyFrameStrings[10*bones_anim_index+u][0], " :()");
////                if (i==debug_ind) {
////                    cout<<keyFrameStrings[10*bones_anim_index+u];
////                    cout<<cmd2<<" ";
////                }
//                char* out = strtok(NULL, " :()");
////                if (i==debug_ind) {
////                    cout<<"hit";
////                }
//                while (out != NULL)
//                {
//                    int time = atoi(out);
//                    float value = atof(strtok(NULL, " :()"));
//                    int size = bones_rest[i].keyPositions[u].size();
//                    //cout<<"size is "<<size<<", time is "<<time<<"\n";
//                    if (size<=time)
//                    {
//                        bones_rest[i].keyPositions[u].resize(time+100);
//                    }
//
//                    bones_rest[i].keyPositions[u][time]=KeyFrame(time, value);
//                    out = strtok(NULL, " :()");
////                    if (i==debug_ind) {
////                        cout<<"("<<time<<":"<<value<<") ";
////                    }
//                }
////                if (i==debug_ind) {
////                    cout<<"\n";
////                }
//            }
//            // switch z, y loc, 1, 2;
////            vector<KeyFrame> temp1 = bones_rest[i].keyPositions[1];
////            bones_rest[i].keyPositions[1] = bones_rest[i].keyPositions[2];
////            bones_rest[i].keyPositions[2] = temp1;
////
////            // switch z, y quatRot, 5, 6;
////            vector<KeyFrame> temp5 = bones_rest[i].keyPositions[5];
////            bones_rest[i].keyPositions[5] = bones_rest[i].keyPositions[6];
////            bones_rest[i].keyPositions[6] = temp5;
////
////            // switch z, y scale, 5, 6;
////            vector<KeyFrame> temp8 = bones_rest[i].keyPositions[8];
////            bones_rest[i].keyPositions[8] = bones_rest[i].keyPositions[9];
////            bones_rest[i].keyPositions[9] = temp8;
//
//            // corrected this
//
//
//            slerped_rots.push_back(Quat(1, 0, 0, 0));
//            lerped_posits.push_back(vec3(0, 0, 0));
//            bone_matrices.push_back(mat4(1.0));
//            bone_delta_mats.push_back(mat4(1.0));
//            Ds.push_back(mat4(1.0));
//        }
//        // debug
//        // cout<<bones_rest[root_index].name;
//
//        vec3 head2head = bones_rest[root_index].head;
////        vec4 head2head4 = vec4(head2head.x, head2head.y, head2head.z, 0.0);
////
////        vec3 head2tail3 = bones_rest[root_index].tail-bones_rest[root_index].head;
////        vec4 head2tail4 = vec4(head2tail3.x, head2tail3.y, head2tail3.z, 0.0);
////
//        mat4 T = Transform::translate(head2head);
////
////        mat3 roll_rot = Transform::rotate(rolls[root_index], vec3(0, 1, 0));
////        mat4 roll_rot4 = mm::mat3_to_mat4(roll_rot);
////
////        vec3 head_tail_rot_axis3 = glm::normalize(glm::cross(vec3(0, 1, 0), head2tail3)); // This cross product might go the wrong way
////        float head_tail_rot_angle = 180/3.14159265*acos(head2tail3.y/(glm::length(head2tail3))); // this in degrees or radians?
////        mat4 head_tail_rot = mm::mat3_to_mat4(Transform::rotate(head_tail_rot_angle, head_tail_rot_axis3));
////
////        mat4 rotation_parent_space = mm::matMult(head_tail_rot, roll_rot4);
////
////        bones_rest[root_index].rest_matrix = mm::matMult(T, rotation_parent_space);
////        bones_rest[root_index].rest_del_mat = mm::matMult(T, rotation_parent_space); // this is wrong!!!
////
////
////        rec_bone_matrices_rest_rolls(root_index, rolls);
//
////        mm::matPrint(T);
////        mm::matPrint(bone_rot_rest[root_index]);
//        bones_rest[root_index].rest_matrix = mm::matMult(T, bone_rot_rest[root_index]);
//        bones_rest[root_index].rest_del_mat = bones_rest[root_index].rest_matrix;
//
//        rec_bone_matrices_rest(root_index, bone_rot_rest);
//
//        norm_matrices.resize(bone_matrices.size());
//
//        // now that all bones have gotten children and parents; the bone matrices can be multiplied together
//
//        std::cout << "bones loaded: " << file_path << "\n";
//
//    }
//    else
//    {
//        cerr << "Unable to Open File " << file_path << "\n" ;
//        /// throw 2 ;
//    }
//}
//
//
///// END PROPOSED NEW IMPLEMENTATION
