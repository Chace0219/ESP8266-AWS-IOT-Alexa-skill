from __future__ import print_function
import re
import json
import traceback
import boto3

### id definitions for relays on devices.
FIRSTRELAY = '1'
SECONDRELAY = '2'
THIRDRELAY = '3'
FOURTHRELAY = '4'
FIFTHRELAY = '5'
SIXTHRELAY = '6'
SEVENTHRELAY = '7'
EIGTHRELAY = '8'


# as you can see in function, you can changer response each relays as you need.
# --------------- Helpers that build all of the responses ----------------------
def build_speechlet_response(title, output, reprompt_text, should_end_session):
    pitchRate = '+0%'
    speedRate = '100%'

    ssmloutput = '<speak><prosody pitch="' + pitchRate
    ssmloutput += '"><prosody rate="'
    ssmloutput += speedRate
    ssmloutput += '">'
    ssmloutput += output
    ssmloutput += '</prosody></prosody></speak>'

    ssmlreprompt = '<speak><prosody pitch="' + pitchRate
    ssmlreprompt += '"><prosody rate="'
    ssmlreprompt += speedRate
    ssmlreprompt += '">'
    ssmlreprompt += reprompt_text
    ssmlreprompt += '</prosody></prosody></speak>'

    return {

        'outputSpeech': {
            'type': 'SSML',
            'ssml': ssmloutput
        },
        'card': {
            'type': 'Simple',
            'title': "SessionSpeechlet - " + title,
            'content': "SessionSpeechlet - " + output
        },
        'reprompt': {
            'outputSpeech': {
                'type': 'SSML',
                'ssml': ssmlreprompt
            }
        },
        'shouldEndSession': should_end_session
    }


def build_response(session_attributes, speechlet_response):
    return {
        'version': '1.0',
        'sessionAttributes': session_attributes,
        'response': speechlet_response
    }


# --------------- Functions that control the skill's behavior ------------------

def get_welcome_response():
    """ If we wanted to initialize the session to have some attributes we could
    add those here
    """

    session_attributes = {}
    card_title = "Welcome"
    # speech_output = "What is your question?"
    speech_output = "what?"

    reprompt_text = "I am ready, Please tell me your answer"

    should_end_session = False
    return build_response(session_attributes, build_speechlet_response(
        card_title, speech_output, reprompt_text, should_end_session))


def handle_session_end_request():
    card_title = "Session Ended"
    speech_output = "Thank you for trying the elly conversation. Have a nice day! "
    # Setting this to true ends the se ssion and exits the skill.
    should_end_session = True
    return build_response({}, build_speechlet_response(
        card_title, speech_output, '', should_end_session))

#########################################################
def relay_control_intenet(intent, session):
    client = boto3.client('iot-data')
    thingsClient = boto3.client('iot')

    card_title = intent['name']
    should_end_session = False
    speech_output = ''
    reprompt_text = ''

    try:
        if 'shadow' in intent['slots']:
            # shadow = intent['slots']['shadow']['value']
            shadow = 'testdevice'
            # int type motor id
            motorid = intent['slots']['motorid']['value']
            new_position = intent['slots']['position']['value']

            ### check thing name in iot devies
            thingsResult = thingsClient.list_things()
            things = thingsResult['things']
            mythings = filter(lambda thing: thing['thingName'] == shadow, things)

            # if there is thing name
            if len(mythings) > 0:
                response = client.get_thing_shadow(
                    thingName=shadow
                )
                state = json.loads(response['payload'].read())
                if motorid == 1:
                    position = state['state']['reported']['motor1']['position']
                    action = state['state']['reported']['motor1']['action']
                    if action != 'idle':
                        speech_output = 'motor ' + str(motorid) + ' is in active state'
                        reprompt_text = speech_output
                    elif position == new_position:
                        speech_output = 'motor ' + str(motorid) + ' is already in desired position'
                        reprompt_text = speech_output
                    else:
                        update = dict()
                        desired = dict()
                        motor = dict()
                        action = dict()
                        action['position'] = new_position
                        motor['motor2'] = action
                        desired['desired'] = motor
                        update['state'] = desired
                        response = client.update_thing_shadow(
                            thingName=shadow,
                            payload=json.dumps(update)
                        )
                        # here
                        speech_output = 'motor ' + str(motorid) + ' will be act to ' + str(new_position)
                        reprompt_text = speech_output

                elif motorid == 2:
                    position = state['state']['reported']['motor2']['position']
                    action = state['state']['reported']['motor2']['action']
                    if action != 'idle':
                        speech_output = 'motor ' + str(motorid) + ' is in active state'
                        reprompt_text = speech_output
                    elif position == new_position:
                        speech_output = 'motor ' + str(motorid) + ' is already in desired position'
                        reprompt_text = speech_output
                    else:
                        update = dict()
                        desired = dict()
                        motor = dict()
                        action = dict()
                        action['position'] = new_position
                        motor['motor1'] = action
                        desired['desired'] = motor
                        update['state'] = desired
                        response = client.update_thing_shadow(
                            thingName=shadow,
                            payload=json.dumps(update)
                        )
                        # here
                        speech_output = 'motor ' + str(motorid) + ' will be act to ' + str(new_position)
                        reprompt_text = speech_output
                elif motorid == 3:
                    position = state['state']['reported']['motor3']['position']
                    action = state['state']['reported']['motor3']['action']
                    if action != 'idle':
                        speech_output = 'motor ' + str(motorid) + ' is in active state'
                        reprompt_text = speech_output
                    elif position == new_position:
                        speech_output = 'motor ' + str(motorid) + ' is already in desired position'
                        reprompt_text = speech_output
                    else:
                        update = dict()
                        desired = dict()
                        motor = dict()
                        action = dict()
                        action['position'] = new_position
                        motor['motor3'] = action
                        desired['desired'] = motor
                        update['state'] = desired
                        response = client.update_thing_shadow(
                            thingName=shadow,
                            payload=json.dumps(update)
                        )
                        # here
                        speech_output = 'motor ' + str(motorid) + ' will be act to ' + str(new_position)
                        reprompt_text = speech_output
                elif motorid == 4:
                    position = state['state']['reported']['motor4']['position']
                    action = state['state']['reported']['motor4']['action']
                    if action != 'idle':
                        speech_output = 'motor ' + str(motorid) + ' is in active state'
                        reprompt_text = speech_output
                    elif position == new_position:
                        speech_output = 'motor ' + str(motorid) + ' is already in desired position'
                        reprompt_text = speech_output
                    else:
                        update = dict()
                        desired = dict()
                        motor = dict()
                        action = dict()
                        action['position'] = new_position
                        motor['motor4'] = action
                        desired['desired'] = motor
                        update['state'] = desired
                        response = client.update_thing_shadow(
                            thingName=shadow,
                            payload=json.dumps(update)
                        )
                        # here
                        speech_output = 'motor ' + str(motorid) + ' will be act to ' + str(new_position)
                        reprompt_text = speech_output
                else:
                    speech_output = 'there is no relay on your device'
                    reprompt_text = speech_output

            else:  # if there is no thing name you find
                speech_output = 'there is no device you find'
                reprompt_text = speech_output

        else:
            speech_output = 'invalid voice command'
            reprompt_text = speech_output
    except:
        speech_output = 'it happens error.'
        reprompt_text = speech_output

    return build_response({}, build_speechlet_response(
        card_title, speech_output, reprompt_text, should_end_session))


######################################################################

#########################################################
def get_status_intenet(intent, session):
    # when you give voice command to get status any relay on thing
    client = boto3.client('iot-data')
    thingsClient = boto3.client('iot')
    card_title = intent['name']
    should_end_session = False

    speech_output = ''
    reprompt_text = ''

    try:
        if 'shadow' in intent['slots']:

            # shadow = intent['slots']['shadow']['value']
            shadow = 'testdevice'
            # int type motor id
            motorid = intent['slots']['motorid']['value']

            ###
            thingsResult = thingsClient.list_things()
            things = thingsResult['things']
            mythings = filter(lambda thing: thing['thingName'] == shadow, things)

            if len(mythings) > 0:
                response = client.get_thing_shadow(
                    thingName=shadow
                )
                state = json.loads(response['payload'].read())
                position = 0
                action = ''
                if motorid == 1:
                    position = state['state']['reported']['motor1']['position']
                    action = state['state']['reported']['motor1']['action']
                elif motorid == 2:
                    position = state['state']['reported']['motor2']['position']
                    action = state['state']['reported']['motor2']['action']
                elif motorid == 3:
                    position = state['state']['reported']['motor3']['position']
                    action = state['state']['reported']['motor3']['action']
                elif motorid == 4:
                    position = state['state']['reported']['motor4']['position']
                    action = state['state']['reported']['motor4']['action']

                speech_output = 'motor ' + str(motorid)+ ' is in ' + action + ', ' + str(position)
                reprompt_text = speech_output
            else:
                speech_output = 'there is no device you find'
                reprompt_text = speech_output
        else:
            speech_output = "invalid voice command"
            reprompt_text = "invalid voice command"
    except:
        speech_output = 'error happens on proecss'
        reprompt_text = speech_output

    return build_response({}, build_speechlet_response(
        card_title, speech_output, reprompt_text, should_end_session))


######################################################################

# --------------- Events ------------------

def on_session_started(session_started_request, session):
    """ Called when the session starts """

    print("on_session_started requestId=" + session_started_request['requestId']
          + ", sessionId=" + session['sessionId'])


def on_launch(launch_request, session):
    """ Called when the user launches the skill without specifying what they
    want
    """

    print("on_launch requestId=" + launch_request['requestId'] +
          ", sessionId=" + session['sessionId'])
    # Dispatch to your skill's launch
    return get_welcome_response()


def on_intent(intent_request, session):
    """ Called when the user specifies an intent for this skill """

    print("on_intent requestId=" + intent_request['requestId'] +
          ", sessionId=" + session['sessionId'])

    intent = intent_request['intent']
    intent_name = intent_request['intent']['name']

    if intent_name == "RelayControlIntent":
        return relay_control_intenet(intent, session)
    elif intent_name == "GetRelayStatus":
        return get_status_intenet(intent, session)
    elif intent_name == "RelayOpenIntent":
        return relay_openclose_intent(intent, session)
    elif intent_name == "AMAZON.HelpIntent":
        return get_welcome_response()
    elif intent_name == "AMAZON.CancelIntent" or intent_name == "AMAZON.StopIntent":
        return handle_session_end_request()
    else:
        raise ValueError("Invalid intent")


def on_session_ended(session_ended_request, session):
    """ Called when the user ends the session.

    Is not called when the skill returns should_end_session=true
    """
    print("on_session_ended requestId=" + session_ended_request['requestId'] +
          ", sessionId=" + session['sessionId'])
    # add cleanup logic here


# --------------- Main handler ------------------

def lambda_handler(event, context):
    """ Route the incoming request based on type (LaunchRequest, IntentRequest,
    etc.) The JSON body of the request is provided in the event parameter.
    """
    print("event.session.application.applicationId=" +
          event['session']['application']['applicationId'])

    """
    Uncomment this if statement and populate with your skill's application ID to
    prevent someone else from configuring a skill that sends requests to this
    function.
    """
    # if (event['session']['application']['applicationId'] !=
    #         "amzn1.echo-sdk-ams.app.[unique-value-here]"):
    #     raise ValueError("Invalid Application ID")

    if event['session']['new']:
        on_session_started({'requestId': event['request']['requestId']},
                           event['session'])

    if event['request']['type'] == "LaunchRequest":
        return on_launch(event['request'], event['session'])
    elif event['request']['type'] == "IntentRequest":
        return on_intent(event['request'], event['session'])
    elif event['request']['type'] == "SessionEndedRequest":
        return on_session_ended(event['request'], event['session'])