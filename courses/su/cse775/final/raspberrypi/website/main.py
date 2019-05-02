#!/usr/bin/env python

import os, sys, time, json, hashlib
from socket import AF_INET, AF_INET6

from gpiozero import MCP3008, DigitalOutputDevice, DigitalInputDevice
import psutil 
from functools import wraps
from flask import Flask, session, request, redirect, url_for, render_template
from flask_socketio import SocketIO, send, emit

LOAD_CELL_DOUT = 19
LOAD_CELL_SCK  = 26
LOAD_CELL_GAIN = 128

app = Flask(__name__)
app.secret_key = "a7Fi2*91_g2JAz5X"
socketio = SocketIO(app)

salt = 'a60H3u1x'
config = {}
config_file = os.path.dirname(os.path.realpath(__file__)) + '/../config.json'
with open(config_file, 'r') as f:
  config = json.load(f)

if not 'username' in config or not config['username']:
  sys.exit('Invalid configuration file.')
if not 'password' in config or not config['password']:
  sys.exit('Invalid configuration file.')

def convert_twos_complement(value):
    return (value & 0x7fffff) - (value & 0x800000)

# Decorated functions
def login_required(f):
  @wraps(f)
  def decorated_function(*args, **kwargs):
    if not session.get('logged_in'):
      return redirect(url_for('login'))
    return f(*args, **kwargs)
  return decorated_function

# Load configuration files
# Load user credentials
# Check if the user is first-time login

@app.route('/')
def hello_world():
  if not session.get('logged_in'):
    return redirect(url_for('login'))
  else:
    return redirect(url_for('status'))

@app.route('/login', methods=['GET', 'POST'])
def login():
  if request.method == 'POST':
    username = str(request.form['username'])
    password = str(request.form['password'])

    md5 = hashlib.md5()
    md5.update((password + salt).encode('utf-8'))
    password = md5.hexdigest()
    # Load user credential when program loads
    if username == config['username'] and password == config['password']:
      session['logged_in'] = True
      session['username'] = username
      return redirect(url_for('status'))
    else:
      return render_template('login.html', errors='Invalid username or password.')
  else:
    return render_template('login.html') 

@app.route('/signout', methods=['POST'])
@login_required
def do_signout():
  session['logged_in'] = False
  session['username'] = ''
  return redirect(url_for('login'))

@app.route('/status', methods=['GET'])
@login_required
def status():
  messages = {}
  data = {}

  data['netcards'] = []
  netcards = psutil.net_if_addrs()
  for key in netcards: 
    netcard = {'name': key, 'mac': '', 'ipv4': {}, 'ipv6': {}}
    for addr in netcards[key]:
      if addr.family == psutil.AF_LINK:
        netcard['mac'] = addr.address
      if addr.family == AF_INET:
        netcard['ipv4'] = { 'address': addr.address, 'netmask': addr.netmask }
      if addr.family == AF_INET6:
        netcard['ipv6'] = { 'address': addr.address, 'netmask': addr.netmask }
    data['netcards'].append(netcard)

  return render_template('status.html', username=session['username'], messages=messages, data=data)

@app.route('/account', methods=['GET'])
@login_required
def account():
  data = { 'key': config['key'], 'email': config['email'] }
  messages = {}
  if session.get('messages'):
    messages = session.get('messages')
    session['messages'] = {}
  return render_template('account.html', username=session['username'], messages=messages, data=data)

@app.route('/updatecredential', methods=['POST'])
@login_required
def do_update_credential():
  global config
  messages = {}
  curpwd = request.form['curpwd']
  newpwd = request.form['newpwd']
  retypepwd = request.form['retypepwd']

  md5 = hashlib.md5()
  md5.update((curpwd + salt).encode('utf-8'))
  curpwd = md5.hexdigest()

  if not curpwd or not newpwd or not retypepwd:
    messages['pwd'] = 'Password cannot be blank.'
  elif newpwd != retypepwd: 
    messages['pwd'] = 'Please confirm your new password again. It must be identical to your new password.'
  elif curpwd != config['password']:
    messages['pwd'] = 'Current password is incorrect.'
  else:
    try:
      with open(config_file, 'r+') as f:
        md5 = hashlib.md5()
        print(newpwd)
        md5.update((newpwd + salt).encode('utf-8'))
        newpwd = md5.hexdigest()
        print(newpwd)
        new_config = config
        new_config['password'] = newpwd
        f.seek(0)
        json.dump(new_config, f)
        config = new_config
        print(config)
        messages['success'] = 'Password updated.'
    except:
      messages['pwd'] = 'Cannot save the password.'
    
  session['messages'] = messages
  return redirect(url_for('account'))

@app.route('/updateemail', methods=['POST'])
@login_required
def do_update_email():
  global config
  data = { 'email': config['email'] }
  messages = {}

  try:
    with open(config_file, 'r+') as f:
      new_config = config
      new_config['email'] = request.form['email']
      f.seek(0)
      json.dump(new_config, f)
      config = new_config
      messages['success'] = 'Email updated.'
  except:
    messages['email'] = 'Cannot save the email.'
  
  session['messages'] = messages
  return redirect(url_for('account'))

@app.route('/updateservicekey', methods=['POST'])
@login_required
def do_update_service_key():
  global config
  data = { 'key': config['key'] }
  messages = {}

  try:
    with open(config_file, 'r+') as f:
      new_config = config
      new_config['key'] = request.form['servicekey']
      f.seek(0)
      json.dump(new_config, f)
      config = new_config
      messages['success'] = 'Service key updated.'
  except:
    messages['key'] = 'Cannot save the service key.'
  
  session['messages'] = messages
  return redirect(url_for('account'))

@app.route('/debug')
@login_required
def debug():
  data = {}
  messages = {}
  return render_template('debug.html', messages=messages, data=data)

@socketio.on('connect', namespace='/debug')
@login_required
def debug_connect():
  emit('connect response', {'data':'Client Connected.'})

@socketio.on('start', namespace='/debug')
def debug_start(message):
  start_time = time.time()
  sample_length = 10
  timeout = 2

  d_out = DigitalInputDevice(pin=LOAD_CELL_DOUT, pull_up=True)
  sck = DigitalOutputDevice(pin=LOAD_CELL_SCK)
  adc = MCP3008(channel=0)

  sck.off()
  counter = 1
  emit_data = []
  while time.time() - start_time < sample_length:
    # Retrieve data from MCP3008
    fsr_data = adc.value

    # Retrieve data from HX711
    loop_start = time.time()
    while not d_out.is_active:
      if time.time() - loop_start > timeout:
        break

    data = 0
    for j in range(24):
      data = data << 1
      sck.on()
      sck.off()
      if d_out.value == 0:
        data = data + 1

    #loadcell_data = {'hex': hex(data), 'num': convert_twos_complement(data)}
    #emit('debug data', {'data': {'fsr': fsr_data, 'loadcell': loadcell_data}})
    #counter = counter + 1
    #if counter % 8 == 0:
    #  emit('debug data', {'data': emit_data})
    #  emit_data = []
    #else:
    #  emit_data.append({'fsr': fsr_data, 'loadcell': convert_twos_complement(data)})
    emit_data.append({'fsr': fsr_data, 'loadcell': convert_twos_complement(data)})
    
    # 25th pulse
    sck.on()
    sck.off()

    # 128 pulses
    for i in range(LOAD_CELL_GAIN):
      sck.on();
      sck.off();
  emit('debug data', {'data': emit_data})
  time.sleep(1)
  emit('debug finish')

@socketio.on('disconnect', namespace='/debug')
def debug_disconnect():
  print('Client disconnected')

if __name__ == "__main__":
  socketio.run(app, host='0.0.0.0', port=13303)
