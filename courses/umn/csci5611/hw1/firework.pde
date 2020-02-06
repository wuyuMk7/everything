import java.util.*;

// Camera control
float cam_radius = 2000;
float eyeX = 0, eyeY = 0, eyeZ= cam_radius, 
      center_x = 0, center_y = 0, center_z = 0;
float angle_x = 0, angle_y = 0;

// Global settings
int scene = 1;

// Custom vector class
class MyVector {
  public float x, y, z;
  
  MyVector() {}
  MyVector(float x, float y, float z) {
    this.x = x;
    this.y = y;
    this.z = z;
  }
  MyVector(MyVector v) {
    x = v.x;
    y = v.y;
    z = v.z;
  }
  
  public void add(MyVector v) {
    x += v.x;
    y += v.y;
    z += v.z;
  }
  
  public void scale(float p) {
    x *= p;
    y *= p;
    z *= p;
  }
  
  public float dot(MyVector v) {
    return x * v.x + y * v.y + z * v.z;
  }
}

// Particle system
class Particle implements Comparable<Particle>{
  public MyVector position = new MyVector();
  public MyVector velocity = new MyVector();
  public float[] colors = new float[4];
  public float size = 5;
  public float lifetime;
  public int type = 0; //0 for up, 1 for explosion

  Particle() {}

  Particle(Particle par)
  {
    position = new MyVector(par.position);
    velocity = new MyVector(par.velocity);
    colors = par.colors.clone();
    size = par.size;
    lifetime = par.lifetime;
    type = par.type;
  }
  
  public int compareTo(Particle par)
  {
    return Float.compare(lifetime, par.lifetime);
  }
}

// Settings
float fireworkRadius = 10;
float expGenRate = 3500, upGenRate = 60; // Rate unit: particles/frame
float maxLife = 10;

// Particles
float[][] fireworkEmitters = new float[3][4]; // 0 - recover time, 1-3 - x,y,z
float explodeY = -200, fireworkUpSpeed = 5;
ArrayList<Particle> explosionPars = new ArrayList<Particle>();
ArrayList<Particle> upPars = new ArrayList<Particle>();

// Control
char keyBeingPressed = '\0';
float pressBeginning = 0;

// Texture
PImage smokeTexture;

void mouseDragged() 
{
  float moveX = mouseX - pmouseX, moveY = mouseY - pmouseY;
  float camRadius = sqrt(sq(eyeX) + sq(eyeY) + sq(eyeZ));
  float angleVertical = asin(eyeY / camRadius) + 1 * moveY * 2 * PI / 360;
  float angleHorizontal = atan2(eyeZ, eyeX) + 1 * moveX * 2 * PI / 360;
  
  if (angleVertical < - PI / 2) angleVertical = - PI / 2 + 0.001;
  if (angleVertical > PI / 2) angleVertical = PI / 2 - 0.001;
  
  eyeY = camRadius * sin(angleVertical);
  eyeX = camRadius * cos(angleVertical) * cos(angleHorizontal);
  eyeZ = camRadius * cos(angleVertical) * sin(angleHorizontal);
}

void mouseWheel(MouseEvent event)
{
  int moveCam = (event.getCount()) * 50;
  float camRadius = sqrt(sq(eyeX) + sq(eyeY) + sq(eyeZ));
  float newCamRadius = camRadius;
  
  if (camRadius + moveCam > 2000) {
    newCamRadius = 2000;
  } else if (camRadius + moveCam < 500) {
    newCamRadius = 500;
  } else {
    newCamRadius = camRadius + moveCam;
  }
  
  if (newCamRadius != camRadius) {
    eyeY = newCamRadius * (eyeY / camRadius);
    eyeX = (sqrt(sq(newCamRadius) - sq(eyeY))) * (eyeX / sqrt(sq(eyeX) + sq(eyeZ)));
    eyeZ = eyeZ > 0 ? sqrt(sq(newCamRadius) - sq(eyeY) - sq(eyeX)) : - sqrt(sq(newCamRadius) - sq(eyeY) - sq(eyeZ));
  }
}

void setup() 
{
  size(1200, 800, P3D); 
  //smokeTexture = loadImage("smoke.png");
  
  for (int i = 0;i < fireworkEmitters.length; ++i) 
    initializeFireworkEmitter(i);
}

void setupLights()
{
  pointLight(200, 200, 200, 300, 0, 100);
  ambientLight(150, 150, 150);
}

void draw() 
{
  float startFrame = millis();
  int totalParticles = 0;
  
  camera(eyeX, eyeY, eyeZ,center_x,center_y,center_z,0,1,0);
  background(25,25,25);
  //setupLights();
  
  strokeWeight(1);
  
  pushMatrix();
  fireworkParticleEmitter();
  updateFirework();
  totalParticles = renderFirework();
  popMatrix();
  
  float endPhysics = millis();
  float endFrame = millis();
  String runtimeReport = 
        "Total particles: "+str(totalParticles)+", "+ 
        "Frame: "+str(endFrame-startFrame)+"ms,"+
        " Physics: "+ str(endPhysics-startFrame)+"ms,"+
        " FPS: "+ str(round(frameRate)) +"\n";
  surface.setTitle("Particle system  -  " +runtimeReport);
}

void fireworkParticleEmitter() 
{
  for (int j = 0;j < fireworkEmitters.length; ++j) {
    if (fireworkEmitters[j][0] > 0) {
      fireworkEmitters[j][0]--;
    } else {
      float curEX = fireworkEmitters[j][1], 
            curEY = fireworkEmitters[j][2],
            curEZ = fireworkEmitters[j][3];
      float numParticles = 0;
      MyVector center = new MyVector(curEX, curEY, curEZ);
      
      if (curEY <= explodeY) {
        numParticles = expGenRate;
        initializeFireworkEmitter(j);
        fireworkEmitters[j][0] = 260;
        for (int i = 0;i < numParticles; ++i) {
          Particle par = new Particle();
          
          float r = fireworkRadius * sqrt(random(0, 1.0)), 
                theta = 2 * PI * random(-1.0, 1.0);

          par.position.x = curEX + r * cos(theta);
          par.position.y = curEY + sqrt(sq(fireworkRadius) - sq(r));
          par.position.z = curEZ + r * sin(theta);
          par.size = 8;
          
          float volFactor = random(0, 10);
          par.velocity.x = (par.position.x - curEX) * 3 * volFactor;
          par.velocity.y = (par.position.y - curEY) * 3 * volFactor;
          par.velocity.z = (par.position.z - curEZ) * 3 * volFactor;
 
          par.colors[0] = 80 * (3 - j);
          par.colors[1] = 80 * j;
          par.colors[2] = 80 * j;
          
          if (j == 0) {
            par.colors[0] = 255;
            par.colors[1] = 43;
            par.colors[2] = 170;
          } else if (j == 1) {
            par.colors[0] = 255;
            par.colors[1] = 248;
            par.colors[2] = 43;          
          } else {
            par.colors[0] = 255;
            par.colors[1] = 156;
            par.colors[2] = 43;          
          }
          par.colors[3] = 220 * random(0.8, 0.9);
          
          par.type = 1;
          par.lifetime = 0;
          
          Particle par2 = new Particle(par);
          par2.position.scale(-1);
          par2.position.add(center);
          par2.position.add(center);
          par2.velocity.x = (par2.position.x - curEX) * 3 * volFactor;
          par2.velocity.y = (par2.position.y - curEY) * 3 * volFactor;
          par2.velocity.z = (par2.position.z - curEZ) * 3 * volFactor;
          
          upPars.add(par);    
          upPars.add(par2);
        }
      } else {
        numParticles = upGenRate;
        fireworkEmitters[j][2] -= fireworkUpSpeed;
        for (int i = 0;i < numParticles; ++i) {
          Particle par = new Particle();
          float theta = 2 * PI * random(-1.0, 1.0);
          
          par.position.x = curEX;
          par.position.y = curEY;
          par.position.z = curEZ;
          par.size = 8;
          
          par.velocity.x = 20 * sin(theta);
          par.velocity.y = 10;
          par.velocity.z = 20 * cos(theta);
          
          par.colors[0] = 252;
          par.colors[1] = 200;
          par.colors[2] = 38;
          par.colors[3] = 120;
          
          par.type = 0;
          par.lifetime = 0;
          upPars.add(par);
        }
      }
    }
  } 
}

void updateFirework()
{
  //println(handPars.size());
  float dt = 1 / frameRate;
  for (int i = 0;i < upPars.size(); ++i) {
    //println(i, handPars.size());
    if (upPars.get(i).lifetime >= maxLife || upPars.get(i).colors[3] <= 1) {
      upPars.remove(i);
    } else {
      Particle par = upPars.get(i);
      //println(par.velocity.x, par.velocity.y, par.velocity.z);
      MyVector displacement = new MyVector(par.velocity);
      displacement.scale(dt);
      par.position.add(displacement);
      
      if (par.type == 0) {
        if (par.lifetime <= 0.15 * maxLife) {
          par.velocity.y += 9.8 * dt;
          
          par.colors[0] *= 0.99;
          par.colors[1] *= 0.98;
          par.colors[2] *= 0.98;
          par.colors[3] *= 0.95;
        } else {
          par.colors[3] *= 0.95;
          //par.size *= 1.001;
          par.velocity.x = random(-1, 1) > 0 ? noise(2) : -noise(2);
          par.velocity.z = random(-1, 1) > 0 ? noise(2) : -noise(2);
          par.velocity.y = noise(2);
        }
      } else {
        par.velocity.y += 9.8 * dt;
        par.colors[3] *= 0.98;
      }
      
      par.lifetime += dt;
    }
  }
}

int renderFirework()
{
  int totalParticles = 0;
  
  /*
  pushStyle();
  beginShape(POINTS); 
  for (Particle par: upPars) {
    if (par.type == 0) continue;
    strokeWeight(10);
    stroke(par.colors[0], par.colors[1], par.colors[2], par.colors[3]);
    
    float parX = par.position.x, parY = par.position.y, parZ = par.position.z;
    vertex(parX - 0.1, parY - 0.1, parZ - 0.1);
    //vertex(parX + 0.1, parY + 0.1, parZ + 0.1);
    ++ totalParticles;
  }
  endShape();
  popStyle();
  
  pushStyle();
  beginShape(POINTS);
  for (Particle par: upPars) {
    if (par.type == 1) continue;
    strokeWeight(par.size);
    stroke(par.colors[0], par.colors[1], par.colors[2], par.colors[3]);
    
    float parX = par.position.x, parY = par.position.y, parZ = par.position.z;
    vertex(parX, parY, parZ);
    ++ totalParticles;
  }
  endShape();
  popStyle();
  */
  
  beginShape(POINTS);
  Collections.sort(upPars);
  for (Particle par: upPars) {
    strokeWeight(par.size);
    stroke(par.colors[0], par.colors[1], par.colors[2], par.colors[3]);
    
    float parX = par.position.x, parY = par.position.y, parZ = par.position.z;
    vertex(parX, parY, parZ);
    ++ totalParticles;
  }  
  endShape(POINTS);
  
  return totalParticles;
}

void initializeFireworkEmitter(int id) 
{
  if (id == 0) {
    fireworkEmitters[0][0] = 0;
    fireworkEmitters[0][1] = -500;
    fireworkEmitters[0][2] = 800;
    fireworkEmitters[0][3] = 0;
  }
  
  if (id == 1) {
    fireworkEmitters[1][0] = 100;
    fireworkEmitters[1][1] = 0;
    fireworkEmitters[1][2] = 800;
    fireworkEmitters[1][3] = 0;  
  }
  
  if (id == 2) {
    fireworkEmitters[2][0] = 200;
    fireworkEmitters[2][1] = 500;
    fireworkEmitters[2][2] = 800;
    fireworkEmitters[2][3] = 0; 
  }
}

void drawCylinder(
  float center_x, float center_z, float cylinder_height, float shape_radius) 
{
  beginShape(QUAD_STRIP);
  for (int i = 0;i < 720; ++i) {
    float theta = 2 * PI * i / 720, 
          ptr_x = center_x + shape_radius * cos(theta),
          ptr_z = center_z + shape_radius * sin(theta);
    
    vertex(center_x, 0, center_z);
    vertex(center_x, cylinder_height, center_z);
    vertex(ptr_x, 0, ptr_z);
    vertex(ptr_x, cylinder_height, ptr_z);
  }
  endShape(CLOSE);
}

void drawCylinder(
  float center_x, float center_z, float cylinder_height, 
  float shape_radius1, float shape_radius2) 
{
  float[][] outer_ptrs = new float[720][2], inner_ptrs = new float[720][2];
  for (int i = 0;i < 720; ++i) {
    float theta = 2 * PI * i / 720;
    outer_ptrs[i][0] = center_x + shape_radius1 * cos(theta);
    outer_ptrs[i][1] = center_z + shape_radius1 * sin(theta);
    inner_ptrs[i][0] = center_x + shape_radius2 * cos(theta);
    inner_ptrs[i][1] = center_z + shape_radius2 * sin(theta);
  }
  beginShape(QUAD_STRIP);
  for (int i = 0;i < 720; ++i) {
    float theta = 2 * PI * i / 720, 
          ptr_x1 = center_x + shape_radius1 * cos(theta),
          ptr_z1 = center_z + shape_radius1 * sin(theta),
          ptr_x2 = center_x + shape_radius2 * cos(theta),
          ptr_z2 = center_z + shape_radius2 * sin(theta);
          
    vertex(ptr_x1, 0, ptr_z1);
    vertex(ptr_x1, cylinder_height, ptr_z1);
    vertex(ptr_x2, 0, ptr_z2);
    vertex(ptr_x2, cylinder_height, ptr_z2);
  }
  endShape(CLOSE);
}
