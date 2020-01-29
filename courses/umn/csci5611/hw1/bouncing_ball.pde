// Shapes initial data
float velocity = 0;
float pos_z = 100, pos_x = 300, pos_y = 300;
float box_pos_x = 300, box_pos_y = 550;
float box_x = 200, box_y = 100, box_z = 200;
float sph_pos_x = 300, sph_pos_y = 100;
float radius = 40;

// Camera control
float cam_radius = 1000;
float eye_x = 300, eye_y = 300, eye_z = cam_radius + pos_z, 
      center_x = 300, center_y = 300, center_z = pos_z;
float angle_x = 0, angle_y = 0;
float floor = box_pos_y - box_y/2;

void setup() 
{
 size(600, 600, P3D);
}

void computePhysics(float dt)
{
  float acceleration= 9.8;
  velocity= velocity + acceleration * dt;
  sph_pos_y += velocity * dt;
  if (sph_pos_y + radius> floor) {
    sph_pos_y = floor-radius;
    velocity *= -.95;
  }
}

void setupLights()
{
  pointLight(200,200,200, 300, 0, 100);
  ambientLight(150, 150, 150);
}

void mouseDragged() {
 float dx = mouseX - pmouseX, dy = mouseY - pmouseY;

 angle_x += dx * 2 * PI / 360;
 angle_y -= dy * 2 * PI / 360;
 eye_x = center_x + cam_radius * cos(angle_y) * sin(angle_x);
 eye_y = center_y - cam_radius * sin(angle_y);
 eye_z = cam_radius * cos(angle_y) * cos(angle_x) + pos_z;
 println(eye_x + " " + eye_y + " " + eye_z);
}

void draw() 
{
  camera(eye_x, eye_y, eye_z,center_x,center_y,center_z,0,1,0);
  background(255,255,255);
  computePhysics(.1);
  setupLights();

  pushMatrix();
  translate(sph_pos_x, sph_pos_y, pos_z); 
  fill(255, 204, 51); 
  noStroke(); 
  sphere(radius);
  popMatrix();
  
  pushMatrix();
  translate(box_pos_x, box_pos_y, pos_z);
  fill(122, 0, 15);
  noStroke();
  box(box_x, box_y, box_z);
  popMatrix();
}
