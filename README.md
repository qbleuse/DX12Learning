# _DX12Learning_

A project to learn DirectX12 and help other people learn DirectX12 in the process.

The project is divided into demos to separate chapters in the learning process.

It is a project made by a student as homework during his studies, you may want a real tutorial but it may come as a good way to start.

Here is an example of the result of the last demo:
![intro_image.png](media/Screenshots/intro_image.png)

___

## Table of Contents

- [_DX12Learning_](#dx12learning)
  - [Table of Contents](#table-of-contents)
  - [Techs](#techs)
  - [How to Build](#how-to-build)
  - [How to Run](#how-to-run)
  - [Additionnal Notes](#additionnal-notes)
  - [License](#license)
- [_DirectX12 Tutorial_](#directx12-tutorial)
  - [Chapters](#chapters)
  - [Initialization](#initialization)
    - [__DirectX Device__](#directx-device)
    - [__Command Queues__](#command-queues)
    - [__Swap Buffer__](#swap-buffer)
    - [__Back Buffer__](#back-buffer)
    - [__Command Lists__](#command-lists)
    - [__Fences__](#fences)
  - [Hello Triangle](#hello-triangle)
    - [__Shaders__](#shaders)
    - [__Pipeline__](#pipeline)
      - [___Root Signature___](#root-signature)
      - [___Creating the Pipeline___](#creating-the-pipeline)
      - [___Creating the Vertex Buffer___](#creating-the-vertex-buffer)
  - [Textured Quad](#textured-quad)
  - [Model](#model)
  - [Skybox](#skybox)
  - [Wrapping Up](#wrapping-up)
  - [Credits](#credits)

___

## Techs

Here is a list of the techs used in this project:

- GLFW for the window manager
- ImGUI (for dx12) for simple UI
- GPM (a math library made by one of my collegue in the [GPEngine](https://github.com/GP-Engine-team/GP_Engine) project)
- tiny_gltf for model loading which itself depends on:
  - stb_image for image loading
  - json.hpp for model parsing
- the CMake solution given by VisualStudio 2019 with clang-cl for windows and Ninja to build(see CMakeSettings.json)
- d3dx12.hpp and DDSTextureLoader12 for faster setup

___

## How to Build

The project has been compiled using the support of Visual Studio for CMake mainly with Ninja, and clang-cl on windows, I guarantee at least that it will compile using this method.

The CMakeSettings.json is available to you. you may change the compiler and the generator if you do not want to use clang and Ninja.

So steps are:

- Install the CMake support for VisualStudio 2019
- Open the folder containing the project with VisualStudio 2019
- Visual should recognize the CMakeSettings.jso file and will open the CMake prompt
- you may close it and choose a debug target
- then click on it to build (or go in Build/Rebuild All)

___

## How to Run

After building, the executable should be in the bin folder, in a folder named from the setting name of your CMakeSettings DebugTarget. its name is DX12Learning.exe.

No pre-compiled executable are given with the project, so, you must build theproject yourself.
See [How to Build](#how-to-build) to have some tips and advice on how to build the project.

___

## Additionnal Notes

- The demos have been designed the simplest as possible making that most things are hardcoded. All that is being done with DirectX12 is correct nevertheless, wich was the whole point.
- I am aware of the UI bug that makes that you cannot rotate on x and that if you do, all the other fields become buggy. It comes from the fact that GPM was originally designed for openGL, and I was not able to find a fix to the problem during the time making. Just don't touch it. Or fix it, your call.
- Scale resets rotation, must come from the same problem. Once Again, do as you wish but I won't fix it.

___

## License

See the License.md file for license rights and limitations.

License of the actual code would be the Unlicensed. However the [AntiqueCamera](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/AntiqueCamera) model comes from the [GLTFSample](https://github.com/KhronosGroup/glTF-Sample-Models) repository, for this see the licensing of the repo.

___

# _DirectX12 Tutorial_

Finally we're here! After all the mandatory repository explanation, we can finally go in the core of the subject: the tutorial.

But before that some disclaimers:

- This tutorial is aimed for people who already knows a graphic API (OpenGL/DirectX11) and usually want to learn a new generation low level API (like Metal, Vulkan, DirectX12) and chose DirectX12. I may talk a bit about it but usually I won't explain how a graphic API works (such as the pipeline).
- I chose to use the least of d3x12.h, which should speed up the initialization process, to be more explicit on how to use the original API (and see what is possible with it). You may want to do some more research on it if you want to be faster at using the API and achieveing your goal.
- You might have already understood from the image above but we go until we have a model and a skybox. that's not much but that is, I think, all you really need to know to learn DirectX12 (at least for graphic pipeline). All of the pipeline stuff (such as rasterizer or blending) are the same as DirectX11 so you should not be disoriented if you know them. If you don't, I advise you follow DirectX11 Tutorials on the subject (the only thing that changes is the way it is created and setted, you'll just have to translate it to the "DX12 way of doing things").
- Here, I won't go into the needy-greedy of things. I advise you see the code if it doesn't work for you.

Let's get just right into it, shall we?

___

## Chapters

- [Initialization](#initialization)
- [Hello Triangle!](#hello-triangle)
- [Textured Quad](#textured-quad)
- [Model](#model)
- [Skybox](#skybox)
- [Wrapping Up](#wrapping-up)
- [Credits](#credits)

___

## Initialization

Firstly, let's talk about the design behind DirectX12 and modern low level graphic API.

The more you go forward in time, the more the graphic card improved in computationnal power. Using the full capacity of this computationnal power sometimes led directly to the performance some programs needed to run fast enough (tipically games). More than that, the graphic card is now used in a lot of programs to do other things that just graphic work (by using things like compute shader or using other low level language for GPU ex: CUDA/OpenCL).

 General consensus was to use this power until you got a bottleneck could be a CPU or GPU. you'd try to optimize it until you could really not because limited by hardware (bandwidth is big part of it). But sometimes, a more problematic bottleneck was discovered in programs: driver based or API based. Problem being, work done by the API under the hood was to slow for the programs on GPU and CPU to go as fast as they could be.

 Therefore, the solution thought by the people designing the API was to simply remove all code that was "unnecessary", meaning that the API should resemble how the GPU works, and be changed to favor how the GPU would work faster, and the APIs are just here to traduce and send infos to the drivers.

 And it worked! kind of. So the problem was solved but people need to learn these, and that is what i am here for.

### __DirectX Device__

 So initializing DirectX12 is fairly similar to its 11 counterpart:

 ``` cpp
    HRESULT hr;

 /*=== Create the device ===*/
 hr = D3D12CreateDevice(
  nullptr,
  D3D_FEATURE_LEVEL_12_0,
  IID_PPV_ARGS(&_device)
 );

 if (FAILED(hr))
 {
  printf("Failing creating DX12 device: %s\n", std::system_category().message(hr).c_str());
  return false;
 }
 
 ```

HRESULT is for the error messages, I won't show it after this but consider it is here everytime for debug purposes.

The device is the object that represents the API and that enables us to create all the other items in the API.

### __Command Queues__

[ID3D12CommandQueue](https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nn-d3d12-id3d12commandqueue)

In previous Graphics API, you would issue your commands when you would need it in your program (such as draw commands).

But every time a command is issued it would be sent straightaway and done immediatly which would say that for each single command we would ask the two processor to stop to wait each other, while you just want command to be done together, following each other.

Queues were designed for this. It helps make a list of commands that follows each other and only access and send to the GPU once (usually all commands for a frame). It also helps with multi-threading (arguably that was the main purpose) as it means you can create commands from different queues from different thread, then throw them in the right order.

 ``` cpp
    /* automatically fills up with default values, and we use those */
 D3D12_COMMAND_QUEUE_DESC cqDesc = {};

 hr = _device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&_queue));
 ```

We're seing a struct for the first time but that is the common way of creating object in DirectX12, you fill up your struct with the infos you want to create your object, then give it to the API to create it.

A cool feature of a lot of object is the NodeMask in those struct to help design multi-GPU architecture.

See [D3D12_COMMAND_QUEUE_DESC](https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_command_queue_desc) to see the options available.

### __Swap Buffer__

[IDXGISwapChain4](https://docs.microsoft.com/en-us/windows/win32/api/dxgi/nn-dxgi-idxgiswapchain)

That is one part of the Tutorial that is not really from the API but we'll go over it

 ``` cpp

 hr = CreateDXGIFactory1(IID_PPV_ARGS(&_factory));
 
 /*===== Create the Swap Chain =====*/

 /* describe the back buffer(s) */
 DXGI_MODE_DESC backBufferDesc = {};
 backBufferDesc.Width = windowWidth;
 backBufferDesc.Height = windowHeight;
 backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

 /* describe the multi sample, we don't use it but unfortunately,
 we are forced to set up to default */
    DXGI_SAMPLE_DESC _sampleDesc = {};
 _sampleDesc.Count = 1;

 /* Describe the swap chain. */
 DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
 swapChainDesc.BufferCount  = bufferCount;
 swapChainDesc.BufferDesc  = backBufferDesc;
 swapChainDesc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
 swapChainDesc.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard the buffer (data) after we call present
 swapChainDesc.OutputWindow = glfwGetWin32Window(window); // handle to our window
 swapChainDesc.SampleDesc = _sampleDesc; // our multi-sampling description
 swapChainDesc.Windowed  = true; // we can change to full screen later


 hr = _factory->CreateSwapChain(
  _queue, // the queue will be flushed once the swap chain is created
  &swapChainDesc, // give it the swap chain description we created above
  (IDXGISwapChain**)&_swapchain // store the created swap chain in a temp IDXGISwapChain interface
 );
 
 ```

We use the the DXGI API to create our Windows swapchain.
Important thing: new API does not accept a bufferCount of 1, it is at least 2 (I advise 3). It was prefered anyway but it will definitly add complexity to our implementation.

Other than that, pretty straightforward.

### __Back Buffer__

Now that we have allocated space, let's get are back buffer. In DirectX12 (and very known Engined, such as UE4 and Unity), a texture that you write on is called a RenderTarget, so we need to retrieve our textures to enable writing on them.

For that we need to do a descriptor heap. So we need to talk about descriptors. I'll take you for a ride!

DX12 says in its documentation:
> "A descriptor heap is a collection of contiguous allocations of descriptors, one allocation for every descriptor."

DX12 also says:
> "Descriptors are the primary unit of binding for a single resource in D3D12."

So, consider we allocated our resources by creating our swapchain. It does not mean the API knows what we want to do with it, we therefore need to _describe_ what it is for. It is basically the way to implement logic on our resources.

So, we allocate descriptor that binds a resource to a way of using it that we put into descriptor heaps to be contiguous in memory, meaning multiple descriptors of different resources next to each other.

But why is it good then? Imagine you have two different shaders with different textures, constant buffer (for OpenGL folks, that's uniform buffers), etc... with different sizes and dimensions, and at different places.

In an old API you would need to rebind all of your resources whereas, here, you can change of all of these bindings in one command by just changing the descriptor heap.

If that does not sound OP for you, it definitely sounds OP for me.

That is an example of changing the API in favor of how the GPU works internally/would be faster.

[ID3D12DescriptorHeap](https://docs.microsoft.com/en-us/windows/win32/direct3d12/descriptor-heaps)

Here is the creation of the descriptor heap:

``` cpp
 /*===== Create the Descriptor Heap =====*/

   /* describe an rtv descriptor heap and create */
 D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
 rtvHeapDesc.NumDescriptors    = bufferCount;
 rtvHeapDesc.Type      = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // it is for Render Target View

 hr = _device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_backBufferDescHeap));

 /* create a depth stencil descriptor heap so we can get a pointer to the depth stencil buffer */
 D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
 dsvHeapDesc.NumDescriptors = 1;
 dsvHeapDesc.Type   = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
 hr = _device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&_depthBufferDescHeap));

 _depthBufferCPUHandle = _depthBufferDescHeap->GetCPUDescriptorHandleForHeapStart();
 _context.depthBufferHandle = _depthBufferCPUHandle;

 /* get the size of a descriptor in this heap descriptor sizes may vary from device to device,
  * which is why there is no set size and we must ask the  device to give us the size.
  * we will use this size to increment a descriptor handle offset */
 _backbufferDescOffset = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

 /* get a handle to the first descriptor in the descriptor heap to create render target from swap chain buffer */
 D3D12_CPU_DESCRIPTOR_HANDLE backBufferCPUHandle(_backBufferDescHeap->GetCPUDescriptorHandleForHeapStart());
 D3D12_GPU_DESCRIPTOR_HANDLE backBufferGPUHandle(_backBufferDescHeap->GetGPUDescriptorHandleForHeapStart());


 /*===== Create the Back Buffers Render Targets =====*/

 _backbuffers.resize(bufferCount);
 _backbufferCPUHandles.resize(bufferCount);
 _backbufferGPUHandles.resize(bufferCount);

 /* Create a render target view for each back buffer */
 for (int i = 0; i < bufferCount; i++)
 {
  _backbufferCPUHandles[i] = backBufferCPUHandle;
  _backbufferGPUHandles[i] = backBufferGPUHandle;
  backBufferCPUHandle.ptr += _backbufferDescOffset;
  backBufferGPUHandle.ptr += _backbufferDescOffset;
 }
 
```

Most of resources in DX12 are binded using [D3D12_CPU_DESCRIPTOR_HANDLE](https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_cpu_descriptor_handle), so we save them as we're bound to use them a lot.

[ID3D12Resource](https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nn-d3d12-id3d12resource)

Them getting the resources and binding descriptor is as follow:

``` cpp
 D3D12_CPU_DESCRIPTOR_HANDLE backBufferCPUHandle(_backBufferDescHeap->GetCPUDescriptorHandleForHeapStart());

 int bufferCount = _backbuffers.size();

 /* Create a render target view for each back buffer */
 for (int i = 0; i < bufferCount; i++)
 {
  /* first we get the n'th buffer in the swap chain and store it in the n'th
   * position of our ID3D12Resource array */
  hr = _swapchain->GetBuffer(i, IID_PPV_ARGS(&_backbuffers[i]));

  /* the we "create" a render target view which binds the swap chain buffer(ID3D12Resource[n]) to the rtv handle */
  _device->CreateRenderTargetView(_backbuffers[i], nullptr, backBufferCPUHandle);

  backBufferCPUHandle.ptr += _backbufferDescOffset;
 }
```

Get the resource, create the descriptor, pretty simple.

### __Command Lists__

[ID3D12GraphicsCommandList4](https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nn-d3d12-id3d12graphicscommandlist)

[ID3D12CommandAllocator](https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nn-d3d12-id3d12commandallocator)

We've already reviewed the new command system earlier so we need two things to store our commands: command allocators and command lists.

``` cpp

/*===== Create the Command Allocators =====*/
 _cmdAllocators.resize(bufferCount);

 for (int i = 0; i < bufferCount; i++)
 {
  hr = _device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocators[i]));
  if (FAILED(hr))
  {
   printf("Failing creating DX12 command allocator nb %d: %s\n", i, std::system_category().message(hr).c_str());
   return false;
  }
 }

 /*===== Create the Command Lists =====*/
 _cmdLists.resize(bufferCount);

 for (int i = 0; i < bufferCount; i++)
 {
  hr = _device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocators[i], NULL, IID_PPV_ARGS(&_cmdLists[i]));
  if (FAILED(hr))
  {
   printf("Failing creating DX12 command List nb %d: %s\n", i, std::system_category().message(hr).c_str());
   return false;
  }

  _cmdLists[i]->Close();
 }
 
```

So why do we create command lists and allocators to the number of back buffers?

Well, firstly, there should be as much allocators as you want command list because they come in pair. As for why I think it is pretty self explanatory.

So doing it with a single command list could work: it is just that CPU would be always waiting for GPU and GPU would be always waiting for CPU. You would issue your command and wait for the command to be available to use, which it would not be. This is the same as if we would create multiple backbuffers, but would only use one: it would work but would be very ineffective.

With DX12, you are able to do things in parallel way more easily than previous instances.
CPU and GPU are two processors working in parallel, we should use this at our advantage.
So even being on a single thread, you could create a command list per frame and, regarless of wether the GPU finished working on the previous frame, you could create the commands for the next one.
Making the CPU is always working and only waiting if the GPU is too much behind.

So this is why, we create to be effective.
But you could be even more effective by creating more command lists to do multithread the command creating process. and it would basically be the same as what we do now just with more of them and mutex for when issuing your command through your queues (which there can be also multiple of if multithreaded).

### __Fences__

So, that's a funny one two.

In DirectX12, synchronisation between precessor is left to the user through the means of fences and event.
What that mean is the synchronisation between CPU and GPU is not automatic like it was before but you need to make fences to be sure that you're not writing over something that is being used are drawn on.
It was chosen as a feature for, of course multithreading but, in majority having an easier time with multi-GPU setup and synchronising them via CPU.

Its more of a bother for us, but is actually pretty cool for others so learn to deal with it and, who knows, maybe one day if you'll have to do it, it would have formed you to do it.

So create our fences, one per backbuffer, not need for more than that in our context. We don't do anything fancy.

[ID3D12Fence](https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nn-d3d12-id3d12fence)

``` cpp

/* Create the Fences */
 _fences.resize(bufferCount);
 _fenceValue.resize(bufferCount);

 for (int i = 0; i < bufferCount; i++)
 {
  
    hr = _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fences[i]));
    
    _fenceValue[i] = 0; // set the initial fence value to 0
 }

 /* create a handle to a fence event */
 _fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
 
```

So the fences work very simply: you have a fence, you set it to reach a certain value when an event is thrown and you simply check if this value is reached when you want to move on.
Otherwise, you just wait.

The _fenceEvent is a HANDLE object that comes from Windows' synchapi.h, an API external of DX12.

We're beginning to have a lot of objects and still no images to see, but where have they gone?

Jokes aside, the big feature of modern graphics API is that a lot of teh work is done prior to realtime, during init ; which make the setup very cumbersome, but very rewarding as draws take very few lines after that (and are very optimized too!).

But that's all for setup of the "Engine part" that won't change or are not setted at runtime.
We can finally move on to do some draws.

Here we comes with graphics world's famous "Hello, World!", the triangle!
___

## Hello Triangle

So, I have bad news. we actually miss a lot to do our triangle. but it is not that difficult so it shouldn't be too long to do.

### __Shaders__

That's the most easy one: triangle shaders. I think i don't need to show what we need, it's pretty straightforward.

However, a cool feature of the hlsl and DirectX in general is if you don't give any vertex buffer to your system (and also don't ask for it) but call a draw indexed, it will give you the index, and with bitwise magic that I definitely would have not come up with, you can make your triangle!

It makes it very fast to draw a triangle and is a technique extremely used for "full screen quad" that is truly a triangle with weird uv. It makes that vertex shader is almost free of cost which is good.

But in our case we'll do it the normal way with vertex buffer to learn.

Here are some shaders for those of you that are lazy.

``` cpp
struct VOut
 {
  float4 position : SV_POSITION;
  float3 color : COLOR; 
 };

 
 VOut vert(float3 position : POSITION, float3 color : COLOR)
 {
  VOut output;
 
  output.position = float4(position,1.0);
  output.color    = color;
 
  return output;
 }

 float4 frag(float4 position : SV_POSITION, float3 color : COLOR) : SV_TARGET
 {
  return float4(color,1.0f);
 }

```

To compile your shader we'll use d3dcompile as for dx11.

``` cpp
D3D12_SHADER_BYTECODE shader_ = {};
ID3DBlob* VSErr, VS_ = nullptr;

 if (FAILED(D3DCompile(shaderSource_.c_str(), shaderSource_.length(), nullptr, nullptr, nullptr, "main", "vs_5_0", SHADER_FLAG, 0, VS_, &VSErr)))
 {
  printf("Failed To Compile Shader %s\n", (const char*)(VSErr->GetBufferPointer()));
  VSErr->Release();
  return false;
 }

 shader_.pShaderBytecode = (*VS_)->GetBufferPointer();
 shader_.BytecodeLength = (*VS_)->GetBufferSize();

```

Same as for HRESULT, for now on VSErr won't appear, but you should still put them in as they show you usefull compilation errors.

### __Pipeline__

Ok, maybe the most important part of this whole tutorial, the pipelines.

So, in DirectX12 (and other modern graphic API, such as Vulkan), there is this notion of pipeline.

Before to change somethings in your pipeline, you would have to create the new state (you may have done it beforehand) then set the new state of the things in your pipeline one by one. That was fairly long and not very optimized. What you can do now is that you can set a whole pipeline at once with a single set command because that you create pipeline object.

The counterpart is that to change a single settings of your pipeline you need to have a whole completely separate pipeline. But the gain of effectiveness for changing pipeline is so big that it does not really matter. It is woud be just very painfull to have to redo a whole pipeline by hand for a single parameter changing. Which we'll be doing.

But the most important parameter of the pipeline is the root signature.

#### ___Root Signature___

Let it be a rendering pipeline. How would it look like if it was a function?

First thing is, Input assembler, rasterizer, vertex shader, pixel shader, etc... would be function called by the pipeline, they're closer to declaration.

Secondly, we've discussed descriptors.
These tipically with the rasterizer settings, the blending settings, etc... would be the parameters.

Then, where do we define the function? That is the root signature. It explains what the function takes into parameters.

To be more fair about the comparison, it is more that shader can take any parameters they want in so the function is a variadic and root signature allows the Pipeline to understand what parameters are given to it.

So root signature is tipically an object containing root parameters and some flags to describe if we ignore steps are not (like we don't use geometry shader so we ignore giving parameter to it).

In our case then, we don't have anything so it will be fairly empty but we need it anyway.

[ID3D12RootSignature](https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nn-d3d12-id3d12rootsignature)

``` cpp

D3D12_ROOT_SIGNATURE_DESC rootDesc = {};
 rootDesc.Flags =  D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
  D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
  D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
  D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
  D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

 hr = D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, &tmp, &error);

 hr = dx12Handle_._device->CreateRootSignature(0, tmp->GetBufferPointer(), tmp->GetBufferSize(), IID_PPV_ARGS(&_rootSignature));

```

So, you might ask yourself: "but if it is only used by the pipeline, why the root signature is its own object?"

It is because a same pipeline might use the same root signature so they made has you can change a pipeline but use the same root signature which makes setting pipeline even faster (as the slowest part is the setting of root access).

#### ___Creating the Pipeline___

After that, it is fairly similar to creating a Pipeline in DirectX11 or other graphics API but in enormous struct format.

[ID3D12PipelineState](https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nn-d3d12-id3d12pipelinestate)

``` cpp
 D3D12_INPUT_ELEMENT_DESC inputLayout[] =
 {
  { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(DemoTriangleVertex, pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
  { "COLOR",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(DemoTriangleVertex, color), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
 };

 // fill out an input layout description structure
 D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};

 // we can get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"
 inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
 inputLayoutDesc.pInputElementDescs = inputLayout;

 D3D12_RASTERIZER_DESC rasterDesc = {};
 rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
 rasterDesc.CullMode = D3D12_CULL_MODE_NONE;

 D3D12_BLEND_DESC blenDesc = {  };

 blenDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


 D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {}; // a structure to define a pso

 psoDesc.InputLayout             = inputLayoutDesc; // the structure describing our input layout
 psoDesc.pRootSignature          = _rootSignature; // the root signature that describes the input data this pso needs
 psoDesc.VS                      = vertex; // structure describing where to find the vertex shader bytecode and how large it is
 psoDesc.PS                      = pixel; // same as VS but for pixel shader
 psoDesc.PrimitiveTopologyType   = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // type of topology we are drawing
 psoDesc.RTVFormats[0]           = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
 psoDesc.SampleDesc              = dx12Handle_._sampleDesc; // must be the same sample description as the swapchain and depth/stencil buffer
 psoDesc.SampleMask              = 0xffffffff; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
 psoDesc.RasterizerState         = rasterDesc; // a default rasterizer state.
 psoDesc.BlendState              = blenDesc; // a default blend state.
 psoDesc.NumRenderTargets        = 1; // we are only binding one render target

 // create the pso
 hr = dx12Handle_._device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pso));

```

#### ___Creating the Vertex Buffer___

Vertex Buffer in DirectX12 are [ID3D12Resource](https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nn-d3d12-id3d12resource)s and same as textures, Unordered access view, regular buffers... So there is this real separation between data and logic.

All the resources are created the same way, we will learn how to do it there.

I have something to tell you... we've being lied. Absolutely bamboozled. Taken for complete fools.

Let me explain you why. What we want to do here is to create a space of data that can contain our resource on the VRAM by allocating and that stays on there (meaning that it is only on the GPU), and send our CPU info into it. We've always been able to do it, how hard could it be?

It happens to be that it is impossible. GPU-only memory can only copy from VRAM.

So, to create our GPU-only vertex buffer, we need to create a second memory on the VRAM that can be accessed by both the CPU and GPU, that we would send the data in, copy in our GPU-Only resources, then be deleted.

And it also happens to be that it was what were doing non-modern graphics API under the hood all along.

Total idiots, I say you...

Here what it looks like:

``` cpp
D3D12_HEAP_PROPERTIES heapProp = {};
 heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

 D3D12_RESOURCE_DESC resDesc = {};

 resDesc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
 resDesc.SampleDesc.Count   = 1;
 resDesc.Width              = bufferData_->SlicePitch;
 resDesc.Height             = 1;
 resDesc.DepthOrArraySize   = 1;
 resDesc.MipLevels          = 1;
 resDesc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

 uploader_.uploadBuffers.resize(uploader_.uploadBuffers.size() + 1);
 hr = uploader_.device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploader_.uploadBuffers.back()));

 heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

 hr = uploader_.device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(resourceData_.buffer));

 UpdateSubresources(uploader_.copyList, *resourceData_.buffer, uploader_.uploadBuffers.back(), 0, 0, 1, bufferData_);

 D3D12_RESOURCE_BARRIER barrier = {};
 barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
 barrier.Transition.pResource   = *resourceData_.buffer;
 barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
 barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
 barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

 uploader_.copyList->ResourceBarrier(1, &barrier);

```

heapProp was the one I was talking about. Other than that resources are the same so we use the same description.

In this, bufferData_ is tipically the resource on CPU you want to send in a [D3D12_SUBRESOURCE_DATA](https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_subresource_data) object.

UpdateSubresources is the only d3dx12 function I used, inside it is a memcpy to the upload buffer followed by a CopyBufferRegion with added padding properties to follow the 16 bytes rule needed for the upload resources (the same that exists with uniform and constant buffer). I have used it for code simplicity.

Also, your resource have states. For example I have created my GPU-only resource with copy dest state because I knew I would do it straightaway and to prevent the use of this resource before it would get initialised.
But I need to change its state. That is what the barrier is for. Barrier are basically GPU Fences to make it wait.

Tipically, as this resource will be bind as vertex buffer the GPU always cheks if its resources are good to go (meaning in its D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER state), and in that case if the resource was in the copy dest state, it would have waited.

I want to point out that all of those are commands from command list so before that we need to open our command list or create a new command list:

``` cpp

hr = dx12Handle_._device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, dx12Handle_._cmdAllocators[dx12Handle_._context.currFrameIndex], NULL, IID_PPV_ARGS(&uploader_.copyList));

```

(note that the TYPE could be copy on this one)

or

``` cpp

 hr = _cmdLists[_currFrameIndex]->Close();
 hr = _cmdAllocators[_currFrameIndex]->Reset();
 hr = _cmdLists[_currFrameIndex]->Reset(_cmdAllocators[_currFrameIndex], NULL);

```

Then we need to issue the command:

``` cpp

 uploader_.copyList->Close();

 ID3D12CommandList* ppCommandLists[] = { uploader_.copyList };
 uploader_.queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

 // Create synchronization objects and wait until assets have been uploaded to the GPU.
 {
    ID3D12Fence* uploadFence;
    hr = uploader_.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&uploadFence));

    // Wait for the command list to execute; 
    // for now, we just want to wait for setup to 
    // complete before continuing.

    // Signal and increment the fence value.
    uploader_.queue->Signal(uploadFence, 1);

    uploadFence->SetEventOnCompletion(1, *uploader_.fenceEvent);
    WaitForSingleObject(*uploader_.fenceEvent, INFINITE);
    uploadFence->Release();
 }
```

We then successfully sent our buffer onto the GPU.

so for our vertex buffer it looks a bit something like this:

``` cpp
/* create vertex buffer resources */
 DemoTriangleVertex triangle[] = {
  { { 0.0f, 0.5f, 0.5f },   { 1.0f, 0.0f, 0.0f }},
  { { 0.5f, -0.5f, 0.5f },  { 0.0f, 1.0f, 0.0f }},
  { { -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }},
 };

 int vBufferSize = sizeof(triangle);

 D3D12_SUBRESOURCE_DATA vertexData = {};
 vertexData.pData        = (BYTE*)(triangle); // pointer to our vertex array
 vertexData.RowPitch     = vBufferSize; // size of all our triangle vertex data
 vertexData.SlicePitch   = vBufferSize; // also the size of our triangle vertex data

 MakeVertexData(cmdList,vertexData);

/* create vertex buffer view out of resources */
_vBufferView.BufferLocation = _vBuffer->GetGPUVirtualAddress();
_vBufferView.StrideInBytes  = sizeof(DemoTriangleVertex);
_vBufferView.SizeInBytes    = vBufferSize;

```

So the logic is in the _vBufferView struct of type [D3D12_VERTEX_BUFFER_VIEW](https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_vertex_buffer_view), while the resource is just a buffer.

That's about it for the vertex buffer.


___

## Textured Quad

___

## Model

___

## Skybox

___

## Wrapping Up

___

## Credits

``` cpp


```
