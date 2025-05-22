# TinyRHI
Todo: write my own render hardware interface about OpenGL and Vulkan

## ExcuteOrder
Pseudocode: 

    define Graphics Engine
    InitHandle
    InitResource

    while(true)
    {
        BeginFrame -> 
            (BeginCommand -> 
                (BeginRenderPass -> setPipeline -> setxxx -> draw -> EndRenderPass) * m -> 
            EndCommand) * n -> 
        EndFrame
    }




