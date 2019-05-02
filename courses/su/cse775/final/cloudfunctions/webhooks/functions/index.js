'use strict';

const functions = require('firebase-functions');
const { DateTime } = require('luxon');

const { dialogflow, SignIn } = require('actions-on-google');
const dialogflowApp = dialogflow({
  clientId: "<ActionsOnGoogle_ClientID>"
});

const firebase = require('firebase');
require('firebase/auth');
require('firebase/firestore');
const sittingloggerConfig = {
  apiKey: "<APIKey>",
  authDomain: "test-auth-55c6f.firebaseapp.com",
  databaseURL: "https://test-auth-55c6f.firebaseio.com",
  projectId: "test-auth-55c6f",
  storageBucket: "test-auth-55c6f.appspot.com",
  messagingSenderId: "<MessageSenderId>"
};
const taskloggerConfig = {
  apiKey: "<APIKey>",
  authDomain: "tasklogger-47837.firebaseapp.com",
  databaseURL: "https://tasklogger-47837.firebaseio.com",
  projectId: "tasklogger-47837",
  storageBucket: "tasklogger-47837.appspot.com"
};
const sittingloggerApp = firebase.initializeApp(sittingloggerConfig, "sittinglogger");
const sittingloggerDb = sittingloggerApp.firestore();
const taskloggerApp = firebase.initializeApp(taskloggerConfig, "tasklogger");
const taskloggerDb = taskloggerApp.firestore();

dialogflowApp.intent('Default Welcome Intent', conv => {
  const { email } = conv.user;
  if (email === undefined) {
    conv.ask("Hi! What can I do for you?");
  } else {
    const payload = conv.user.profile.payload;
    const name = payload ? `${ payload.given_name }` : '';
    conv.ask(`Hi, ${ name }! What can I do for you?`); 
  }
});

dialogflowApp.intent('Default Fallback Intent', conv => {
  conv.ask("I didn't understand. Can you tell me something else?");
});

dialogflowApp.intent('Start Signin', (conv) => {
  conv.ask(new SignIn('To get your account details'));
});

dialogflowApp.intent('Get Signin', (conv, params, signin) => {
  if (signin.status !== 'OK') {
    conv.ask(`Sorry! Authentication failed.`);
  } else {
    const payload = conv.user.profile.payload;
    const name = payload ? `${ payload.given_name }` : '';
    conv.ask(`Authentiation succeeded. `);
    conv.ask(`I got your account details, ${ name }. What do you want to do next?`);
  }
});

dialogflowApp.intent('Sitting Time', async (conv, params) => {
  const { email } = conv.user;
  if (email === undefined) {
    conv.ask(new SignIn("To get more details")); 
  } else {
    const { token } = conv.user.profile;
    const credential = firebase.auth.GoogleAuthProvider.credential(token);

    let todayObject;
    if (params.date == '') {
      todayObject = getToday('America/New_York');
    } else {
      todayObject = DateTime.fromISO(params.date, { setZone: true });
    }
    const today = todayObject.toFormat('yyyy-MM-dd');

    let locale = 'en-US';
    if (conv.user.locale != 'undefined' && conv.user.locale != '')
      locale = conv.user.locale;
    let todayStr = todayObject.setLocale(locale).toLocaleString({ year: 'numeric', month: 'long', day: 'numeric'});

    try {
      const userCredential = await sittingloggerApp.auth().signInAndRetrieveDataWithCredential(credential);
      try {
        const sittingdata = await sittingloggerDb
                                  .collection(userCredential.user.email)
	      			  .doc('sitting-signal')
                                  .collection('raw_data')
	      			  .doc(today)
                                  .get();
        if (!sittingdata.exists) {
          conv.ask(`Cannot find sitting data on ${ todayStr }. `); 
        } else {
          let minute = 0;
	  let dataValue = sittingdata.data();
	  for (let i in dataValue)
	    minute += dataValue[i];
	
	  let message = `Accumulated sitting time on ${ todayStr } is ${ minute } minutes. `;
	  if (minute > 60) {
	    let hour = Math.floor(minute / 60);
	    minute = minute % 60;
	    message = `Accumulated sitting time on ${ todayStr } is ${ hour } hours ${ minute } minutes. `;
	  }
	  conv.ask(message);
        }
      } catch (err) {
          console.log(err);
          let error = new Error('Sitting time logger Database Error!');
          error.name = 'DatabaseError';
          conv.ask(`Sorry! Error occured when retrieving data from Sitting Time Logger database. `);
          conv.ask(`Please try it again. `);
      }
    } catch(err) {
      let error = new Error('Sitting time logger Authentication Failed!');
      error.name = 'AuthenticationError';
      conv.ask(`Sorry! Cannot locate your profile. Please link your Google account to Sitting Time Logger account first.`);
      conv.ask(`If you have already finished account linking but still have issues, please contact Sitting Time Logger support.`);
    }
  }
});

dialogflowApp.intent('To Do List', async (conv) => {
  const { email } = conv.user;
  if (email === undefined) {
    conv.ask(new SignIn("To get more details"));
  } else {
    const { token } = conv.user.profile;
    const credential = firebase.auth.GoogleAuthProvider.credential(token);
    const todayObject = getToday('America/New_York');
    const today = todayObject.toFormat('yyyyMMdd');

    try {
      const userCredential = await taskloggerApp.auth().signInAndRetrieveDataWithCredential(credential);
      try {
        const todoItems = await taskloggerDb
                                  .collection('tasks')
                                  .doc(userCredential.user.email)
                                  .collection(today)
                                  .where('status', '==', String.fromCodePoint(0x1F3A2))
                                  .get();
        if (todoItems.empty) {
          conv.ask(`You don't have any ongoing tasks today!`); 
        } else {
          let counter = 0;
	  let message = "";
          conv.ask(`You have ${ todoItems.size } ongoing tasks today! \n`); 
          todoItems.forEach(todoItem => {
            ++ counter;
	    message += `Task${ counter }: ${ todoItem.data().content }. \n`;
          });
	  conv.ask(message);
        }
      } catch (err) {
          console.log(err);
          let error = new Error('Tasklogger Database Error!');
          error.name = 'DatabaseError';
          conv.ask(`Sorry! Error occured when retrieving data from Task Logger database. `);
          conv.ask(`Please try it again. `);
      }
    } catch(err) {
      let error = new Error('Tasklogger Authentication Failed!');
      error.name = 'AuthenticationError';
      conv.ask(`Sorry! Cannot locate your profile. Please link your Google account to Task Logger account first.`);
      conv.ask(`If you have already finished account linking but still have issues, please contact Task Logger support.`);
    }
  }
});

exports.personalinfo = functions.https.onRequest(dialogflowApp);

function getToday(timezone) {
  if (timezone === undefined || timezone === '')
    return DateTime.utc();
  else
    return DateTime.utc().setZone(timezone);
}

