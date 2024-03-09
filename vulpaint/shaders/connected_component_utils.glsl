#ifndef CONNECTED_COMPONENT_UTILS
#define CONNECTED_COMPONENT_UTILS
#extension GL_EXT_scalar_block_layout: enable
#extension GL_EXT_buffer_reference2 : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int32   : enable
#extension GL_EXT_shader_explicit_arithmetic_types_float32 : enable
layout(buffer_reference, buffer_reference_align= 4, scalar) buffer ConnectedComponentLabels_ref {
    uint data;
};



/**
 * finds the "root", or the parent label of the current label indexed into labels.
 * based on https://github.com/FolkeV/CUDA_CCL?tab=readme-ov-file
 * @param labels global memory array of labels
 * @param label index of labels, must be less than the length of labels.
 * @return the parent label index which does not itself have a parent inside of labels.
 */
uint find_root(ConnectedComponentLabels_ref labels, uint label){
    // the index in labels where we might need to follow the next label.
    uint next_label = labels.data[label];

    //follow chain
    // each value in labels represents a handle to the parent label.
    // each pixel is initialized with a label corresponding to it's own
    // linear address offset, ie value at x = 4, y = 31 would be
    // initialized with 31 * width + 4.   If the label we follow is the same as the
    // current label we know we are at the "end" of the parent chain of labels.
    // we've found the "root" label for the given pixel.
    while(label != next_label){
        label = next_label;
        next_label = labels.data[label];
    }

    return label;
}



/**
 * finds the "root", or the parent label of the current label indexed into labels.
 * based on https://github.com/FolkeV/CUDA_CCL?tab=readme-ov-file
 * @param labels global memory array of labels
 * @param label index of labels, must be less than the length of labels.
 * @return the parent label index which does not itself have a parent inside of labels.
 */
uint merge_labels(ConnectedComponentLabels_ref labels, uint label_1, uint label_2){
    uint next_label_1 = (label_1 != label_2) ? labels.data[label_1] : 0u;
    uint next_label_2 = (label_1 != label_2) ? labels.data[label_2] : 0u;

    while((label_1 != label_2) && (label_1 != next_label_1)){
        //follow chain until label_1 is either the root, or
        //is the same as label_2.
        label_1 = next_label_1;
        next_label_1 =  labels.data[label_1];
    }

    while((label_1 != label_2) && (label_2 != next_label_2)){
        //follow chain until label_2 is either the root, or
        //is the same as label_1.
        label_2 = next_label_2;
        next_label_2 =  labels.data[label_2];
    }

    while(label_1 != label_2){
        if(label_1 < label_2){
            // github code uses bit swap, but there's no way the compiler wouldn't just
            // do that on it's own if it found it faster.  If that's not the case, then
            // that's a compiler bug.  I think in theory it's supposed to prevent an additional
            // register from being used, but straight up telling the compiler what you're
            // actually trying to do, should, in theory, make it easier for the compiler
            // to do it's job.
            uint temp = label_1;
            label_1 = label_2;
            label_2 = label_1;
        }

        //TODO The memory semantic scope of atomic min changes depending on which kernel we run, even if we operate in
        //global memory, we may only need visibility for the current local workgroup

        uint prev_min_label = atomicMin(labels.data[label_1], label_2);
        //atomic min returns the previous value, if label_1 matched the previous value, then we can set to label_2 and exit
        // if it didn't, that means that label_2, which contained the smallest value out of label_1 and label_2 origionally,
        // didn't replace the larger value.
        label_1 = (label_1 == prev_min_label) ? label_2 : prev_min_label;
    }
    //returns the lowest label value, ie root.
    return label_1;
}

#endif //CONNECTED_COMPONENT_UTILS
