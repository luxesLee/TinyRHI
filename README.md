# TinyRHI
Todo: write my own render hardware interface about Vulkan

Unfinished, to be developed

## Processes and Plans

- Current Processes: 
    Build the basic Vulkan operating framework and run the simple test examples. The project still has many issues and some hard-coded elements.

- Plan:
  - Replace the hard-coded elements, expand more Vulkan flags, and enhance the overall framework's robustness.
  - Build the OpenGL code.

## The Simplest Example

```c++
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        pHandle->
            BeginFrame()->
                BeginCommand()->
                    // Attachment need to be set before renderPassBegin to get renderPass and framebuffer
                    SetDefaultAttachments(attachmentDesc)->
                    // SetDepthAttachment()->
                    BeginRenderPass()->
                        SetVertexShader(vertShader)->
                        SetPixelShader(pixelShader)->
                        // Buffer Texture need be set before setpipeline to get pipelineLayout
                        SetGraphicsPipeline(gfxSetting)->
                        SetViewport(Extent2D(0, 0), Extent2D(1024, 1024))->
                        SetScissor(Extent2D(0, 0), Extent2D(1024, 1024))->
                        SetVertexStream(0, pVeretx, 0)->
                        DrawPrimitive(3, 0)->
                    EndRenderPass()->
                EndCommand()->
                Commit()->
            EndFrame();
    }
```

See and run 'test/TinyRHI_min_example.cpp'
