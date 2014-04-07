import functools
import os
import random
import time
import test
from piui import PiUi

current_dir = os.path.dirname(os.path.abspath(__file__))
metadata_file = '../test_headers.txt'
test_names = ["Right eye dilation test","Right eye tracking test","Right eye speed test",
                "Left eye dilation test","Left eye tracking test","Left eye speed test"]
test_descriptions = ["This test will shine a light in the users left eye and measure the pupil dilation",
                    "This test will consist of a sequence of lights that illuminate 1 after another",
                    "This test will consist of collection of lights illuminating in sequence",
                    "This test will shine a light in the athletes right eye and measure pupil dilation",
                    "This test will consist of a sequence of lights that illuminate 1 after another",
                    "This test will consist of collection of lights illuminating in sequence"]
test_instructions = ["Instruct athlete to just look forward at the camera",
                    "Instruct athlete to try to look in the direction of the illuminated light",
                    "Instruct athlete to try to look in the direction of the illuminated lights",
                    "Instruct athlete to just look forward at the camera",
                    "Instruct athlete to try to look in the direction of the illuminated light",
                    "Instruct athlete to try to look in the direction of the illuminated lights"]


class ETGPiUI(object):

    def __init__(self):
        self.title = None
        self.first_name = ''
        self.last_name = ''
        self.img = None
        self.current_test = 1
        self.baseline = None
        self.ui = PiUi(img_dir=os.path.join(current_dir,'imgs'))
    

    def main(self):
        self.main_menu()
        self.ui.done()

    def main_menu(self):
        self.reset_user_values
        self.page = self.ui.new_ui_page(title="Concussion Test")
        box = self.page.add_textbox("Welcome to Group 8's concussion testing program", "h2")
        button = self.page.add_button("Run concussion test", self.form_page)
        self.ui.done()


    def form_page(self):
        self.page = self.ui.new_ui_page(title="Enter your information",prev_text="Back",onprevclick=self.main_menu)
        self.first_name = self.page.add_input("text","First name")
        self.last_name = self.page.add_input("text","Last name")
        self.page.add_textbox("Is this a baseline test?","h3")
        self.page.add_button("Yes",self.yes_base_line_click)
        self.page.add_button("No",self.no_base_line_click)
        self.page.add_element('br')
        self.page.add_button("Submit",self.form_submit)
        self.page.add_element("hr")
        self.title = self.page.add_textbox("",'h1')

    def results_page(self):
	   self.page = self.ui.new_ui_page(title="Test Results",prev_text="Back",onprevclick=self.main_menu)
	   self.img = self.page.add_image("area_graph.png")

    def yes_base_line_click(self):
        self.baseline = True

    def no_base_line_click(self):
        self.baseline = False

    def form_submit(self):
        if self.first_name == '' and self.last_name == '':
            self.title.set_text("Please enter your full name")
        elif self.first_name == '':
            self.title.set_text("Please enter your first name")
        elif self.last_name == '':
            self.title.set_text("Please enter your last name")
        elif self.baseline == None:
            self.title.set_text("Please enter if this is a baseline test")
        else:
            bl_str = ''
            if self.baseline == True:
                bl_str = 'Yes'
            else:
                bl_str = 'No'

            self.title.set_text("Please confirm:\n First: "+ self.first_name +"\nLast: "
                +self.last_name+"\nBaseline: "+bl_str)
            self.page.add_button("Confirm",self.ready_for_test_page)

    def create_request_file(self):
        user_name = self.first_name+"_"+self.last_name
        bl = '1' if self.baseline else '0'
    
        test_num = str(self.current_test)

        f = open(metadata_file,'w')
        f.write(user_name.lower())
        f.write(bl)
        f.writ(test_num)
        f.close()


    def ready_for_test_page(self):
        self.page = self.ui.new_ui_page(title=test_names[self.current_test-1])
        self.txt = self.page.add_textbox(test_descriptions[self.current_test-1])
        self.page.add_element("br")
        self.page.add_textbox(test_instructions[self.current_test-1])
        self.page.add_button("Begin test",self.test_in_progress_page)


    def test_in_progress_page(self):
        self.create_request_file()
        self.page = self.ui.new_ui_page(title="Test In Progress")
        self.page.add_textbox("Testing athlete... Please wait")
        done_file = '../Test_'+str(self.current_test)+'done.txt'
        while(not os.path.exists(done_file)):
            #wait for processing to be complete
            time.sleep(.5)
        if self.current_test != 6:
            self.current_test += 1
            self.ready_for_test_page()
        else:
            self.results_page()

    def reset_user_values(self):
        self.first_name = ''
        self.last_name = ''
        self.current_test = 1
        self.baseline = None


	
def main():
    piui = ETGPiUI()
    piui.main()

if __name__ == '__main__':
    main()

