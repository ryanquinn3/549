import functools
import os
import random
import time
import test
from piui import PiUi

current_dir = os.path.dirname(os.path.abspath(__file__))

class ETGPiUI(object):

    def __init__(self):
        self.title = None
        self.txt = None
        self.img = None
        self.ui = PiUi(img_dir=os.path.join(current_dir, 'imgs'))
        self.src = "sunset.png"

    def main(self):
        self.main_menu()
        self.ui.done()

    def main_menu(self):
        self.page = self.ui.new_ui_page(title="Concussion Test")
        box = self.page.add_textbox("Welcome to Group 8's concussion testing program", "h2")
        button = self.page.add_button("Run concussion test", self.run_test_click)
        self.ui.done()

    def run_test_click(self):
        self.page = self.ui.new_ui_page(title="Run test")
        text = self.page.add_textbox("Running test please wait","h1")
        test.run()
        self.results_page()

    def results_page(self):
	self.page = self.ui.new_ui_page(title="Test Results",prev_text="Back",onprevclick=self.main_menu)
	self.img = self.page.add_image("area_graph.png")
	
def main():
    piui = ETGPiUI()
    piui.main()

if __name__ == '__main__':
    main()

