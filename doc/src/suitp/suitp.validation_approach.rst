Validation Approach
===================

Test-Driver Development (TDD)
------------------------------

The validation by unit testing is the basic of the Development workflow that uses TDD. In the TDD approach, the developer writes a test that defines the desired function or improvement, 
then produces the minimum amount of code to pass that test, and finally refactors the new code to acceptable standards.

This methodology has several advantages:
- it forces the developer to write tests
- it forces the developer to write only the necessary code to pass the test, leading to a test coverage of 100% by Design
- it forces the developer to write code that is easy to test

This last point is particulary important because it forces the developer to think about the software architecture and its modularity from the beginning to make it easy to test,
limiting the modules dependencies and making the code more maintainable.

Validation coverage verification
--------------------------------

Test cases syntax
----------------------

The Test cases use the Gherkin syntax to describe the test cases. The syntax is as follows:

.. code:: gherkin
   
    Scenario: <scenario name>
        Given <precondition>
        When <action>
        Then <expected result>

It makes the test cases easy to read and understand, by clearly identifying :
 - the initial state
 - the action that is performed
 - the expected result


Considerations on Test Terminology
----------------------------------

The test cases are divided into three categories:
 - unit tests
 - software integration tests
 - system integration tests
 - End-to-End tests (E2E)

These tests categories can by represented by a pyramid, stacked from the bottom to the top, with the unit tests at the bottom, the software integration tests, the system integration tests and 
finally, the E2E tests at the top.

The pyramid shape is a representation of the number of tests that should be written for each category. The unit tests are the most numerous, followed by the software integration tests, the system
integration tests and finally the E2E tests that are the less numerous.

Each categories focus on specific aspects of the software validation.

The Unit Tests
^^^^^^^^^^^^^^

The unit tests focus on the software testable parts of the software. It addresses mainly functions and methods testing from a functional point of view, but also the validation
of software modules together with their dependencies as far as it doesn't involved external systems.

The Software Integration Tests
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The software integration tests focus on the functional validation of a part of the software on a representative taget platform. 
It addresses the interactions between the different modules of the software including with external system elements like database, communciation, but also hardware.
This category of tests can include instrumented code, or specific tests features to, for instance, simulate errors or corner cases by fault injection.
Performance tests and measurement tests (WCET) can also be included in this category.

The System Integration Tests
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This category of tests focus the functional behavior of the final software seen as a blackbox with a surrounding test environement. The hardware target platform must be representative 
from the software point of view, but could include specific tests, error injections or observability features to help to validate the software.

These tests are out of the scope of this document, and listed and described in the SATP (Software Acceptance Plan) aka Software Validation Plan w.r.t. Requirement Baseline (RB).

The End-to-End tests (E2E)
^^^^^^^^^^^^^^^^^^^^^^^^^^

The E2E tests focus on the validation of the final software in its final environment, including the hardware, the communication, the external systems and the user interface.
These tests are out of the scope of the software validation, but are important to validate the software in its final environment.
They are part of the system acceptance test plan (ATP), w.r.t. User Requirements Specification (URS).





