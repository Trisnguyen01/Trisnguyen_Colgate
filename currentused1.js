function doGet(e)
{
  Logger.log("--- doGet ---");
 
  var tag = "",value = "";
 
  try 
  {
 
    // this helps during debuggin
    if (e == null){e={}; e.parameters = {tag:"test",value:"-1"};}
 
    tag = e.parameters.tag;
    value = e.parameters.value;
 
    // save the data to spreadsheet
    save_data(tag, value);
 
 
    return ContentService.createTextOutput("Wrote:\n  tag: " + tag + "\n  value: " + value);
 
  } 
  
  catch(error) 
  { 
    Logger.log(error);    
    return ContentService.createTextOutput("oops...." + error.message 
                                            + "\n" + new Date() 
                                            + "\ntag: " + tag +
                                            + "\nvalue: " + value);
  } 

	/*var sheet = SpreadsheetApp.getActiveSheet().setName('Vibralog');
	var headers = [
	'Number',
	'Date',
	'Work type',
	'Frequency (Hz)']; */
	/*var initialData = [
	'350 5th Ave, New York, NY 10118',
	'405 Lexington Ave, New York, NY 10174']; */
	sheet.getRange('A1:D1').setValues([headers]).setFontWeight('bold');
	//sheet.getRange('A2:B2').setValues([initialData]);
	//sheet.setFrozenRows(1);
	sheet.autoResizeColumns(1, 4);  
} //end of function doGet




// Method to save given data to a sheet
function save_data(tag, value)
{
  Logger.log("--- save_data ---"); 
 
   try  {
	var dateTime = new Date();
 
    // Paste the URL of the Google Sheets starting from https thru /edit
    // For e.g.: https://docs.google.com/..../edit 
    var ss = SpreadsheetApp.openByUrl("https://docs.google.com/spreadsheets/d/1t7k9oebWYKI4wjc3vAqcrfucjIMZPNHAhSjXI6L9Tso/edit");
    var dataLoggerSheet = ss.getSheetByName("Vibralog"); //search for the sheet
 
 
    // Get last edited row from Vibralog sheet
    var row = dataLoggerSheet.getLastRow() + 1;
 
 
    // Start Populating the data
    dataLoggerSheet.getRange("A" + row).setValue(row -1); // ID
    dataLoggerSheet.getRange("B" + row).setValue(dateTime); // dateTime
    dataLoggerSheet.getRange("C" + row).setValue(tag); // tag
    dataLoggerSheet.getRange("D" + row).setValue(value); // value
    dataLoggerSheet.getRange("E" + row).setValue(tag); // tag
    //dataLoggerSheet.getRange("D" + row).setValue(value); // value
   
    // Update summary sheet
    summarySheet.getRange("B1").setValue(dateTime); // Last modified date
    // summarySheet.getRange("B2").setValue(row - 1); // Count 
  }
 
  catch(error) 
  {
    Logger.log(JSON.stringify(error));
  }
 
  Logger.log("--- save_data end---"); 
} //end of saveData
function onOpen() {
  var spreadsheet = SpreadsheetApp.getActive();
  var menuItems = [
    {name: 'Prepare sheet...', functionName: 'prepareSheet_'},
    {name: 'Generate step-by-step...', functionName: 'generateStepByStep_'}
  ];
  spreadsheet.addMenu('Cool', menuItems);
} // end of function onOpen





/**
 
 */


/**
 * A function that adds headers and some initial data to the spreadsheet.
 */
function prepareSheet_() {
  var sheet = SpreadsheetApp.getActiveSheet().setName('Vibralog');
  var headers = [
    'Number',
    'Date',
    'Work type',
    'Frequency (Hz)'];
  /*var initialData = [
    '350 5th Ave, New York, NY 10118',
    '405 Lexington Ave, New York, NY 10174']; */
  sheet.getRange('A1:D1').setValues([headers]).setFontWeight('bold');
  //sheet.getRange('A2:B2').setValues([initialData]);
  //sheet.setFrozenRows(1);
  sheet.autoResizeColumns(1, 4);
} //end of function prepareSheet_

/**
 * Creates a new sheet containing step-by-step directions between the two
 * addresses on the "Settings" sheet that the user selected.
 */
function generateStepByStep_() { /*
  var spreadsheet = SpreadsheetApp.getActive();
  var settingsSheet = spreadsheet.getSheetByName('Settings');
  settingsSheet.activate();

  // Prompt the user for a row number.
  var selectedRow = Browser.inputBox('Generate step-by-step',
      'Please enter the row number of the addresses to use' +
      ' (for example, "2"):',
      Browser.Buttons.OK_CANCEL);
  if (selectedRow == 'cancel') {
    return;
  }
  var rowNumber = Number(selectedRow);
  if (isNaN(rowNumber) || rowNumber < 2 ||
      rowNumber > settingsSheet.getLastRow()) {
    Browser.msgBox('Error',
        Utilities.formatString('Row "%s" is not valid.', selectedRow),
        Browser.Buttons.OK);
    return;
  }

  // Retrieve the addresses in that row.
  var row = settingsSheet.getRange(rowNumber, 1, 1, 2);
  var rowValues = row.getValues();
  var origin = rowValues[0][0];
  var destination = rowValues[0][1];
  if (!origin || !destination) {
    Browser.msgBox('Error', 'Row does not contain two addresses.',
        Browser.Buttons.OK);
    return;
  }

  // Get the raw directions information.
  var directions = getDirections_(origin, destination);

  // Create a new sheet and append the steps in the directions.
  var sheetName = 'Driving Directions for Row ' + rowNumber;
  var directionsSheet = spreadsheet.getSheetByName(sheetName);
  if (directionsSheet) {
    directionsSheet.clear();
    directionsSheet.activate();
  } else {
    directionsSheet =
        spreadsheet.insertSheet(sheetName, spreadsheet.getNumSheets());
  }
  var sheetTitle = Utilities.formatString('Driving Directions from %s to %s',
      origin, destination);
  var headers = [
    [sheetTitle, '', ''],
    ['Step', 'Distance (Meters)', 'Distance (Miles)']
  ];
  var newRows = [];
  for (var i = 0; i < directions.routes[0].legs[0].steps.length; i++) {
    var step = directions.routes[0].legs[0].steps[i];
    // Remove HTML tags from the instructions.
    var instructions = step.html_instructions.replace(/<br>|<div.*?>/g, '\n')
        .replace(/<.*?>/g, '');
    newRows.push([
      instructions,
      step.distance.value
    ]);
  }
  directionsSheet.getRange(1, 1, headers.length, 3).setValues(headers);
  directionsSheet.getRange(headers.length + 1, 1, newRows.length, 2)
      .setValues(newRows);
  directionsSheet.getRange(headers.length + 1, 3, newRows.length, 1)
      .setFormulaR1C1('=METERSTOMILES(R[0]C[-1])');

  // Format the new sheet.
  directionsSheet.getRange('A1:C1').merge().setBackground('#ddddee');
  directionsSheet.getRange('A1:2').setFontWeight('bold');
  directionsSheet.setColumnWidth(1, 500);
  directionsSheet.getRange('B2:C').setVerticalAlignment('top');
  directionsSheet.getRange('C2:C').setNumberFormat('0.00');
  var stepsRange = directionsSheet.getDataRange()
      .offset(2, 0, directionsSheet.getLastRow() - 2);
  setAlternatingRowBackgroundColors_(stepsRange, '#ffffff', '#eeeeee');
  directionsSheet.setFrozenRows(2);
  SpreadsheetApp.flush(); */
}  // end of generateStepByStep

/**
 * Sets the background colors for alternating rows within the range.
 * @param {Range} range The range to change the background colors of.
 * @param {string} oddColor The color to apply to odd rows (relative to the
 *     start of the range).
 * @param {string} evenColor The color to apply to even rows (relative to the
 *     start of the range).
 */
function setAlternatingRowBackgroundColors_(range, oddColor, evenColor) {
  var backgrounds = [];
  for (var row = 1; row <= range.getNumRows(); row++) {
    var rowBackgrounds = [];
    for (var column = 1; column <= range.getNumColumns(); column++) {
      if (row % 2 == 0) {
        rowBackgrounds.push(evenColor);
      } else {
        rowBackgrounds.push(oddColor);
      }
    }
    backgrounds.push(rowBackgrounds);
  }
  range.setBackgrounds(backgrounds);
}

/**
 * A shared helper function used to obtain the full set of directions
 * information between two addresses. Uses the Apps Script Maps Service.
 *
 * @param {String} origin The starting address.
 * @param {String} destination The ending address.
 * @return {Object} The directions response object.
 */
function getDirections_(origin, destination) { /*
  var directionFinder = Maps.newDirectionFinder();
  directionFinder.setOrigin(origin);
  directionFinder.setDestination(destination);
  var directions = directionFinder.getDirections();
  if (directions.status !== 'OK') {
    throw directions.error_message;
  }
  return directions; */
} //end of getDirection

// Method to save given data to a sheet


function save_data(tag, value)
{
  Logger.log("--- save_data ---"); 
 
 
  try 
  {
    var dateTime = new Date();
 
    // Paste the URL of the Google Sheets starting from https thru /edit
    // For e.g.: https://docs.google.com/..../edit 
    var ss = SpreadsheetApp.openByUrl("1t7k9oebWYKI4wjc3vAqcrfucjIMZPNHAhSjXI6L9Tso");
    var dataLoggerSheet = ss.getSheetByName("DataLogger");
 
 
    // Get last edited row from DataLogger sheet
    var row = dataLoggerSheet.getLastRow() + 1;
 
 
    // Start Populating the data
    dataLoggerSheet.getRange("A" + row).setValue(row -1); // ID
    dataLoggerSheet.getRange("B" + row).setValue(dateTime); // dateTime
    dataLoggerSheet.getRange("C" + row).setValue(tag); // tag
    dataLoggerSheet.getRange("D" + row).setValue(value); // value
    dataLoggerSheet.getRange("E" + row).setValue(tag); // tag
    dataLoggerSheet.getRange("D" + row).setValue(value); // value
   
    // Update summary sheet
    summarySheet.getRange("B1").setValue(dateTime); // Last modified date
    // summarySheet.getRange("B2").setValue(row - 1); // Count 
  }
 
  catch(error) 
  {
    Logger.log(JSON.stringify(error));
  }
 
  Logger.log("--- save_data end---"); 
} //end of saveData