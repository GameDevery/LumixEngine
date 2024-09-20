//@include "pipelines/common.hlsli"

#ifdef EXTRACT
	cbuffer Data : register(b4) {
		float u_avg_lum_multiplier;
		uint b_histogram;
		uint u_input;
		uint u_output;
	};

	[numthreads(16, 16, 1)]
	void main(uint3 thread_id : SV_DispatchThreadID) {
		float avg_lum = asfloat(bindless_buffers[b_histogram].Load(256 * 4));
		float3 c = bindless_textures[u_input][thread_id.xy * 2].rgb
			+ bindless_textures[u_input][thread_id.xy * 2 + uint2(1, 0)].rgb
			+ bindless_textures[u_input][thread_id.xy * 2 + uint2(1, 1)].rgb
			+ bindless_textures[u_input][thread_id.xy * 2 + uint2(0, 1)].rgb;
		float _luminance = luminance(c * 0.25);

		float multiplier = saturate(1 + _luminance - avg_lum * u_avg_lum_multiplier);
		bindless_rw_textures[u_output][thread_id.xy] = float4(c * multiplier, 1);
	}
#elif defined DOWNSCALE
	cbuffer Data : register(b4) {
		uint u_input;
		uint u_output;
	};

	[numthreads(16, 16, 1)]
	void main(uint3 thread_id : SV_DispatchThreadID) {
		float4 o_color = bindless_textures[u_input][thread_id.xy * 2]
			+ bindless_textures[u_input][thread_id.xy * 2 + uint2(1, 0)]
			+ bindless_textures[u_input][thread_id.xy * 2 + uint2(1, 1)]
			+ bindless_textures[u_input][thread_id.xy * 2 + uint2(0, 1)];
		o_color *= 0.25;
		bindless_rw_textures[u_output][thread_id.xy] = o_color;
	}
#else
	cbuffer Data : register(b4) {
		uint u_input;
		uint u_output;
	};

	[numthreads(16, 16, 1)]
	void main(uint3 thread_id : SV_DispatchThreadID) {
		float2 uv = thread_id.xy / float2(Global_framebuffer_size.xy);
		float4 a = sampleBindlessLod(LinearSamplerClamp, u_input, uv, 0);
		float4 b = bindless_rw_textures[u_output][thread_id.xy];
		bindless_rw_textures[u_output][thread_id.xy] = a + b;
	}
#endif