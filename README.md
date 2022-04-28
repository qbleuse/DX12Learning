# _DX12Learning_

A project to learn DirectX12 and help other people learn DirectX12 in the process.

The project is divided into demos to separate chapters in the learning process.

It is a project made by a student as homework during his studies, you may want a real tutorial but it may come as a good way to start.


Here is an example of the result of the last demo:
![intro_image.png](media/Screenshots/intro_image.png)

___

## Table of Contents

- [DX12Learning](#dx12learning)
    - [Table of Contents](#table-of-contents)
    - [Techs](#techs)
    - [How to Build](#how-to-build)
    - [How to Run](#how-to-run)
    - [Additionnal Notes](#additionnal-notes)
    - [License](#license)
- [DX12 Tutorial](#directx12-tutorial)


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

In previous Graphics API, you would issue your commands when you would need it in your program (such as draw commands).

But every time a command is issued it would be sent straightaway and done immediatly which whould say that for each single command we would ask the two processor to stop to wait each other, while you just want command to be done together, following each other. 

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
	swapChainDesc.BufferCount		= bufferCount;
	swapChainDesc.BufferDesc		= backBufferDesc;
	swapChainDesc.BufferUsage		= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect		= DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard the buffer (data) after we call present
	swapChainDesc.OutputWindow	= glfwGetWin32Window(window); // handle to our window
	swapChainDesc.SampleDesc	= _sampleDesc; // our multi-sampling description
	swapChainDesc.Windowed		= true; // we can change to full screen later


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

Here is the creation of the descriptor heap:

 ``` cpp
	/*===== Create the Descriptor Heap =====*/

   /* describe an rtv descriptor heap and create */
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc	= {};
	rtvHeapDesc.NumDescriptors				= bufferCount;
	rtvHeapDesc.Type						= D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // it is for Render Target View

	hr = _device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_backBufferDescHeap));

	/* create a depth stencil descriptor heap so we can get a pointer to the depth stencil buffer */
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
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

___

## Hello Triangle!

 ``` cpp
 ``` 

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