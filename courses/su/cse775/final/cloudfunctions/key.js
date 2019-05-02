'use strict'

const crypto = require('crypto');
const salt = 'a693c107';

function generate(email, salt) {
  let text = email + salt;
  const hash = crypto.createHash('sha256').update(text).digest()

  return Buffer.from(hash).toString('base64')
}

console.log(generate('test@a.edu', salt));
console.log(generate('yhe106@syr.edu', salt));
console.log(generate('xzhan45@syr.edu', salt));
console.log(generate('xzhan45@gmail.com', salt));
