// the max/min speeds of each boids.
#define MAX_SPEED 2.0f
#define MIN_SPEED 0.60f

// these are the weights for each computation. cohesion should
// always be less than separation
#define separationWeight 1.5f
#define alingmentWeight 5.0f
#define cohesionWeight  0.9f

// this is the radius at which each rule can be applied. Separation should
// be lower than cohesion
#define separationRadius 3.50f
#define alingmentRadius 20.0f
#define cohesionRadius 20.0f

// this angle is half the angle of the cone that they can see
#define coneHalfAngle 90.0f / 3.14159f

// smoothing affects how smoothly that rotate. smaller means that they will take more simulation
// ticks to rotate, so changing this can affect flocking behaviours
#define smoothing 0.00250f
#define numBoids 1024
#define fieldRadius 300

void __kernel computeNextState(global float* position, global float* velocity, global float* outputVel){
if(get_global_id(0)%4  == 0){
	//////////////////////////////////////////////////////////////////////////////////
	int sepCount = 0;
	int alignCount = 0;
	int cohesionCount = 0;
	
	float3 separation =(float3)  (0.0f,0.0f,0.0f);
	float3 alingment  =(float3)  (0.0f,0.0f,0.0f);
	float3 cohesion    =(float3)  (0.0f,0.0f,0.0f);
	
	float3 boidPos   = (float3) (position[get_global_id(0)+0], position[get_global_id(0)+1], position[get_global_id(0)+2]);
	float3 boidVel   = (float3) (velocity[get_global_id(0)+0], velocity[get_global_id(0)+1], velocity[get_global_id(0)+2]);
	
	float velMag = length(boidVel);
	for(int i = 0; i < numBoids;i++){
		int index = (i*4);
		if(index != get_global_id(0)){
			
			float3 pos   = (float3) (position[index+0], position[index+1], position[index+2]);
			float3 vel   = (float3) (velocity[index+0], velocity[index+1], velocity[index+2]);
			
			float3 direction = boidPos - pos;
			float distance = length(direction);
			
			float ang = (dot(direction,boidVel));
			ang = ang/(distance*velMag);
			ang = acos(ang);
			if(ang <=coneHalfAngle ){
				
				if (distance <= separationRadius) {
					separation -= (pos - boidPos);
					sepCount++;
				}
				if (distance <= alingmentRadius) {
					alingment+=vel;
					alignCount++;
				}
				if (distance <= cohesionRadius) {
					cohesion+=pos;
					cohesionCount++;
				}
			}
	
		}
	}
	
	separation *= separationWeight;
	if (alignCount > 0) {
		alingment /= alignCount;
		alingment -= boidVel;
		alingment *= alingmentWeight;
	}

	if (cohesionCount>0) {
		cohesion /= cohesionCount;
		cohesion -= boidPos;
		cohesion *= cohesionWeight;
	}
	
	
	float3 res = separation+alingment+cohesion;

	if (length(boidPos) > fieldRadius) {
		res -= boidPos*0.10f;
	}
	
	outputVel[get_global_id(0)] 	  = res.x;
	outputVel[get_global_id(0)+1]  = res.y;
	outputVel[get_global_id(0)+2]  = res.z;
	outputVel[get_global_id(0)+3]  = 0;

	///////////////////////////////////////////////////////////
}
}

void __kernel doNextStep(global float* position, global float* velocity, global float* inputVel ){
	
	if(get_global_id(0)%4  == 0){
		float3 front   =(float3) (inputVel[get_global_id(0)+0], inputVel[get_global_id(0)+1], inputVel[get_global_id(0)+2]);
		float3 vel   = (float3) (velocity[get_global_id(0)+0], velocity[get_global_id(0)+1], velocity[get_global_id(0)+2]);
		vel = vel + front*smoothing;
		float velMag = length(vel);
		
		
		if(velMag > MAX_SPEED ){
			vel = normalize(vel) * MAX_SPEED;
		}
		else if(velMag < MIN_SPEED){
			vel =normalize(vel) * MIN_SPEED;
		}
		
		position[get_global_id(0)] 	   +=vel.x;
		position[get_global_id(0)+1] +=vel.y;
		position[get_global_id(0)+2] +=vel.z;
		
		velocity[get_global_id(0)] 	   =vel.x;
		velocity[get_global_id(0)+1] =vel.y;
		velocity[get_global_id(0)+2] =vel.z;
	}

}
