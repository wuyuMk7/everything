import java.util.*;

// Shapes initial data
float velocity = 0;
float pos_z = 100, pos_x = 300, pos_y = 300;
float box_pos_x = 300, box_pos_y = 550;
float box_x = 200, box_y = 100, box_z = 200;
float sph_pos_x = 300, sph_pos_y = 100;
//float radius = 40;

// Camera control
float cam_radius = 1000;
float eye_x = 0, eye_y = -100, eye_z = 4000, 
      center_x = 0, center_y = -100, center_z = 0;
float angle_x = 0, angle_y = 0;
float floor = box_pos_y - box_y/2;

// Global settings
int scene = 1;

// Particle system
class Particle implements Cloneable{
  public float[] position = new float[3];
  public float[] velocity = new float[3];
  public float[] colors = new float[4];
  public float size = 5;
  public float lifetime;

  Particle() {}

  Particle(Particle par)
  {
    position = par.position.clone();
    velocity = par.velocity.clone();
    colors = par.colors.clone();
    size = par.size;
    lifetime = par.lifetime;
  }
}

// Settings
float fireRadius = 35;
float genRate = 450, dt = .1;
float maxLife = 25;
// A good lifecycle value = 20

// 
boolean leftTorch = false, rightTorch = false, gateOpened = false;
float ltX = -550, ltY = -400, ltZ = 2150, rtX = 550, rtY = -400, rtZ = 2150;
float htX = 100, htY = -150, htZ = 3600;
int torchDistance;
ArrayList<Particle> leftPars = new ArrayList<Particle>();
ArrayList<Particle> rightPars = new ArrayList<Particle>();
ArrayList<Particle> handPars = new ArrayList<Particle>();

// Control
char keyBeingPressed = '\0';
float pressBeginning = 0;
float walkVelocity = 10;

// Texture
PImage smokeTexture;

void setup() 
{
 size(1200, 800, P3D); 
 //smokeTexture = loadImage("smoke.png");
}

void setupLights()
{
  pointLight(200,200,200, 300, 0, 100);
  ambientLight(150, 150, 150);
}

void movement() 
{
  if (keyPressed) {
    if ((key == 'w' || key == UP) && eye_z >= 700 + walkVelocity) {
      eye_z -= walkVelocity;
      htZ -= walkVelocity;
      for (Particle par: handPars)
        par.position[2] -= walkVelocity * 0.8;
    } else if ((key == 's' || key == DOWN) && eye_z <= 4000 - walkVelocity) {
      eye_z += walkVelocity;
      htZ += walkVelocity;
      for (Particle par: handPars)
        par.position[2] += walkVelocity * 0.8;
    } 
  }
}

void moveTorch()
{
  float curX = htX, curY = htY;
  float nextX = htX + mouseX - pmouseX, nextY = htY + mouseY - pmouseY;
  
  if (nextX < -540) 
    htX = -540;
  else if (nextX > 540)
    htX = 540;
  else
    htX = nextX;
    
  if (nextY > 100)
    htY = 100;
  else if (nextY < -300)
    htY = -300;
  else
    htY = nextY;
    
  for (Particle par: handPars) {
    par.position[0] += (nextX - curX) * 0.9;
    par.position[1] += (nextY - curY) * 0.9;
  }
}

void mouseDragged() {
  moveTorch();
}

void mousePressed()
{       
  if (!leftTorch && (torchDistance == 0 || torchDistance == 2))
    leftTorch = true;
  if (!rightTorch && (torchDistance == 1 || torchDistance == 2))
    rightTorch = true;
    
  if (leftTorch && rightTorch)
    gateOpened = true;
}

// -1 means no one works, 0 means lt can be lighted, 1 means rt can be lighted, 2 means both
int checkTorchDistance()
{
  if (leftTorch && rightTorch)
    return -1;
  
  int ret = -1;
  if (sq(htX - ltX) + sq(htY - ltY) + sq(htZ - ltZ) <= 250000)
    ret = 0;
  if (sq(htX - rtX) + sq(htY - rtY) + sq(htZ - rtZ) <= 200000)
    ret = ret == -1 ? 1 : 2;
  
  return ret;
}

void draw() 
{
  float startFrame = millis();
  int totalParticles = 0;
  torchDistance = checkTorchDistance();
  
  camera(eye_x, eye_y, eye_z,center_x,center_y,center_z,0,1,0);
  background(255,255,255);
  //setupLights();
  
  strokeWeight(1);
  
  pushMatrix();
  rotateX(PI / 2);
  translate(-1000, 0, -800);
  rect(0, 0, 2000, 4000);
  popMatrix();
  
  pushMatrix();
  fill(200, 200, 200);
  renderRoom();
  if (gateOpened)
    renderOpenedGate();
  else
    renderClosedGate();
  popMatrix();

  pushMatrix();
  drawTorches();
  popMatrix();
  
  pushStyle();
  torchFireParticleEmitter();
  updateTorchFire();
  totalParticles = renderTorchFire();
  popStyle();
  
  movement();
 
  float endPhysics = millis();
  float endFrame = millis();
  String runtimeReport = 
        "Total particles: "+str(totalParticles)+", "+ 
        "Frame: "+str(endFrame-startFrame)+"ms,"+
        " Physics: "+ str(endPhysics-startFrame)+"ms,"+
        " FPS: "+ str(round(frameRate)) +"\n";
  surface.setTitle("Particle system  -  " +runtimeReport);
}

void renderRoom() 
{
  /*
  line(1000,0,0,-1000,0,0);
  line(0,800,4000,0,800,0);
  line(0,-800,0,0,800,0);
  */
  
  // Left wall
  translate(-850, 0, 150);
  box(300, 1600, 300);
  translate(-50, 0, 1000);
  box(200, 1600, 1700);
  translate(50, 0, 1000);
  box(300, 1600, 300);
  translate(-50, 0, 1000);
  box(200, 1600, 1700);
  
  // Right wall
  translate(1800, 0, 0);
  box(200, 1600, 1700);
  translate(-50, 0, -1000);
  box(300, 1600, 300);
  translate(50, 0, -1000);
  box(200, 1600, 1700);
  translate(-50, 0, -1000);
  box(300, 1600, 300);
}

void renderOpenedGate()
{
  translate(-225, 0, -425);
  box(150, 1600, 700);
  translate(-1250, 0, 0);
  box(150, 1600, 700);
}

void renderClosedGate()
{
  translate(-500, 0, 0);
  box(700, 1600, 150);
  translate(-700, 0, 0);
  box(700, 1600, 150);
}

void torch(float center_x, float center_y)
{
  stroke(207, 170, 118);
  drawCylinder(center_x, center_y, 50, 20);
  translate(0, 50, 0);
  stroke(64, 38, 2);
  drawCylinder(center_x, center_y, 250, 10);
}

void drawTorches()
{
  push();
  translate(-550, -400, 2150);
  rotate(PI / 6);
  torch(0, 0);
  translate(-50, 50, 0);
  stroke(0, 0, 0);
  box(150, 30, 30);
  pop();
  
  push();
  translate(550, -400, 2150);
  rotate(-PI / 6);
  torch(0, 0);
  translate(50, 50, 0);
  stroke(0, 0, 0);
  box(150, 30, 30);
  pop();
  
  pushMatrix();
  translate(htX, htY, htZ);
  torch(0, 0);
  popMatrix();
}

void torchFireParticleEmitter() 
{
  float numParticles = dt * genRate;
  float fracPart = numParticles - int(numParticles);
  numParticles = int(numParticles);
  
  if (fracPart != 0 && random(-1, 1) >= 0) {
    numParticles += 1;
  }
  
  for (int i = 0;i < numParticles; ++i) {
    float r = 20 * sqrt(random(0, 1.0)) + fireRadius - 20, 
          theta = 2 * PI * random(-1.0, 1.0);
    Particle par = new Particle();

    par.position[0] = htX + r * cos(theta);
    par.position[1] = htY + sqrt(fireRadius * fireRadius - r * r) + 20;
    par.position[2] = htZ + r * sin(theta);
    par.size = 12;

    par.velocity[0] = 5 * random(-0.5, 0.5);
    par.velocity[1] = -10;
    par.velocity[2] = 5 * random(-0.5, 0.5);
    
    par.colors[0] = 252;
    par.colors[1] = 200;
    par.colors[2] = 38;
    par.colors[3] = 120;
    
    par.lifetime = 0;
    handPars.add(par);
    
    if (leftTorch) {
      Particle leftPar = new Particle(par);
      leftPar.position[0] = ltX + r * cos(theta) - 20;
      leftPar.position[1] = ltY + sqrt(fireRadius * fireRadius - r * r) + 25;
      leftPar.position[2] = ltZ + r * sin(theta);
      
      leftPars.add(leftPar);
    }
    
    if (rightTorch) {
      Particle rightPar = new Particle(par);
      rightPar.position[0] = rtX + r * cos(theta) + 20;
      rightPar.position[1] = rtY + sqrt(fireRadius * fireRadius - r * r) + 25;
      rightPar.position[2] = rtZ + r * sin(theta);
      
      rightPars.add(rightPar);
    }
  }
}

void updateTorchFire()
{
  //println(handPars.size());
  for (int i = 0;i < handPars.size(); ++i) {
    //println(i, handPars.size());
    if (handPars.get(i).lifetime >= maxLife || handPars.get(i).colors[3] <= 1) {
      handPars.remove(i);
    } else {
      Particle par = handPars.get(i);
      float moveCloser = random(1, 1.05), dis = sqrt(sq(par.position[0] - htX) + sq(par.position[2] - htZ)), 
            sinTheta = abs(par.position[0] - htX) / dis ,
            cosTheta = abs(par.position[2] - htZ) / dis ;

      if (par.lifetime >= maxLife * 0.4) {
        par.colors[0] *= 0.98;
        par.colors[1] *= 0.98;
        par.colors[3] *= 0.95;
        par.size *= 2;
        
        par.velocity[0] *= (0.9);
        par.velocity[1] *= 0.99;
        par.velocity[2] *= (0.9);
      } else {
        par.colors[1] -= 2;
        par.colors[2] -= 1;
        par.colors[3] *= 0.98;
        
        par.velocity[0] += (par.position[0] > htX ? -(moveCloser * sinTheta * dt) : (moveCloser * sinTheta * dt));
        par.velocity[1] -= random(0.05) * dt;
        par.velocity[2] += (par.position[2] > htZ ? -(moveCloser * cosTheta * dt) : (moveCloser * cosTheta * dt));
      }
     
      par.position[0] += par.velocity[0] * dt;
      par.position[1] += par.velocity[1] * dt;
      par.position[2] += par.velocity[2] * dt;
      
      /* With life = 20, smoke right at the peak
      par.velocity[0] += (par.position[0] > htX ? -(moveCloser * sinTheta * dt) : (moveCloser * sinTheta * dt));
      par.velocity[1] -= random(0.05) * dt;
      par.velocity[2] += (par.position[2] > htZ ? -(moveCloser * cosTheta * dt) : (moveCloser * cosTheta * dt));
        
      par.position[0] += par.velocity[0] * dt;
      par.position[1] += par.velocity[1] * dt;
      par.position[2] += par.velocity[2] * dt;
      
      if (par.lifetime >= maxLife * 0.4) {
        par.colors[3] *= 0.9;
        par.size *= 1.2;
      } else {
        par.colors[1] -= 1.5;
        par.colors[2] -= 1;
        par.colors[3] *= 0.985;
      }
      */
      
      par.lifetime += dt;
    }
  }
  
  if (leftTorch) {
    for (int i = 0;i < leftPars.size(); ++i) {
      //println(i, handPars.size());
      if (leftPars.get(i).lifetime >= maxLife || leftPars.get(i).colors[3] <= 1) {
        leftPars.remove(i);
      } else {
        Particle par = leftPars.get(i);
        float moveCloser = random(1, 1.05), dis = sqrt(sq(par.position[0] - ltX + 20) + sq(par.position[2] - ltZ)), 
              sinTheta = abs(par.position[0] - ltX + 20) / dis ,
              cosTheta = abs(par.position[2] - ltZ) / dis ;
  
        if (par.lifetime >= maxLife * 0.4) {
          par.colors[0] *= 0.98;
          par.colors[1] *= 0.98;
          par.colors[3] *= 0.95;
          par.size *= 2;
          
          par.velocity[0] *= (0.9);
          par.velocity[1] *= 0.99;
          par.velocity[2] *= (0.9);
        } else {
          par.colors[1] -= 2;
          par.colors[2] -= 1;
          par.colors[3] *= 0.98;
          
          par.velocity[0] += (par.position[0] > ltX - 20 ? -(moveCloser * sinTheta * 0.1) : (moveCloser * sinTheta * 0.1));
          par.velocity[1] -= random(0.05) * dt;
          par.velocity[2] += (par.position[2] > ltZ ? -(moveCloser * cosTheta * 0.1) : (moveCloser * cosTheta * 0.1));
        }
        
        par.position[0] += par.velocity[0] * dt;
        par.position[1] += par.velocity[1] * dt;
        par.position[2] += par.velocity[2] * dt;
        par.lifetime += dt;
      }
    }
  }
  
  if (rightTorch) {
    for (int i = 0;i < rightPars.size(); ++i) {
      //println(i, handPars.size());
      if (rightPars.get(i).lifetime >= maxLife || rightPars.get(i).colors[3] <= 1) {
        rightPars.remove(i);
      } else {
        Particle par = rightPars.get(i);
        float moveCloser = random(1, 1.05), dis = sqrt(sq(par.position[0] - rtX - 20) + sq(par.position[2] - ltZ)), 
              sinTheta = abs(par.position[0] - rtX - 20) / dis ,
              cosTheta = abs(par.position[2] - rtZ) / dis ;
  
        if (par.lifetime >= maxLife * 0.4) {
          par.colors[0] *= 0.98;
          par.colors[1] *= 0.98;
          par.colors[3] *= 0.95;
          par.size *= 2;
          
          par.velocity[0] *= (0.9);
          par.velocity[1] *= 0.99;
          par.velocity[2] *= (0.9);
        } else {
          par.colors[1] -= 2;
          par.colors[2] -= 1;
          par.colors[3] *= 0.98;
          
          par.velocity[0] += (par.position[0] > rtX + 20 ? -(moveCloser * sinTheta * dt) : (moveCloser * sinTheta * dt));
          par.velocity[1] -= random(0.05) * dt;
          par.velocity[2] += (par.position[2] > rtZ ? -(moveCloser * cosTheta * dt) : (moveCloser * cosTheta * dt));
        }
        
        par.position[0] += par.velocity[0] * dt;
        par.position[1] += par.velocity[1] * dt;
        par.position[2] += par.velocity[2] * dt;
        par.lifetime += dt;
      }
    }
  }
}

int renderTorchFire()
{
  int totalParticles = 0;
  
  beginShape(POINTS);
  //texture(smokeTexture);
  if (leftTorch) {
    for (Particle par: leftPars) {
      strokeWeight(par.size);
      stroke(par.colors[0], par.colors[1], par.colors[2], par.colors[3]);
      vertex(par.position[0], par.position[1], par.position[2]);
      ++ totalParticles;
    }
  } 
  
  if (rightTorch) {
    for (Particle par: rightPars) {
      strokeWeight(par.size);
      stroke(par.colors[0], par.colors[1], par.colors[2], par.colors[3]);
      vertex(par.position[0], par.position[1], par.position[2]);
      ++ totalParticles;
    }
  } 
  
  for (Particle par: handPars) {
    strokeWeight(par.size);
    stroke(par.colors[0], par.colors[1], par.colors[2], par.colors[3]);
    vertex(par.position[0], par.position[1], par.position[2]);
    ++ totalParticles;
  }
  endShape();
  
  return totalParticles;
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
