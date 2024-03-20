# Monte-Carlo & Importance Sampling
Implemented a Monte-Carlo estimator for the direct lighting equation

1. Uniform Hemisphere Sampling 

Implemented a Monte Carlo estimator where the outgoing direction is uniformly sampled from the upper hemisphere formed at the shading point. Shoot a ray in the sampled direction to check if the closest intersection of the ray is a light source. If the intersection is a light source, then accumulate the reflected radiance according to the estimator. Built an Orthonormal Basis (ONB) at each shading point, which would be used to convert the vectors from shading space to world space and vice-versa. 

2. Cosine Weighted Sampling
Instead of uniformly sampling the hemisphere, used cosine weighted sampling to find the outgoing direction ωo.

3. Light Sampling 
Implemented importance sampling of the lights in the scene. Sampling lights involves two steps: choosing a light to sample from and then choosing a point on the light. Used a simple uniform sampling strategy to choose the light to sample from. Extended the sampling routine to sample the surface of the chosen area light uniformly and used it to implement the final estimator.